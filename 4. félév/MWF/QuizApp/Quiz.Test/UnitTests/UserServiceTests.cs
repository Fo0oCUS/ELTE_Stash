using System.Security.Claims;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Identity;
using Microsoft.Extensions.Options;
using Moq;
using Quiz.DataAccess.Models;
using Quiz.DataAccess.Services;
using Quiz.DataAccess.Services.Config;

namespace Quiz.Test.UnitTests;

[TestClass]
public class UserServiceTests
{
    private UserService _userService = null!;
    private Mock<UserManager<AppUser>> _mockUserManager = null!;
    private Mock<SignInManager<AppUser>> _mockSignInManager = null!;
    private Mock<IHttpContextAccessor> _mockHttpContextAccessor = null!;

    [TestInitialize]
    public void Init()
    {
        _mockUserManager = MockUserManager();
        _mockSignInManager = MockSignInManager(_mockUserManager.Object);
        _mockHttpContextAccessor = new Mock<IHttpContextAccessor>();

        var jwtSettings = Options.Create(new JwtSettings
        {
            SecretKey = "12345678901234567890123456789012",
            Issuer = "en",
            Audience = "te",
            AccessTokenExpirationMinutes = 60
        });

        _userService = new UserService(
            jwtSettings,
            _mockHttpContextAccessor.Object,
            _mockUserManager.Object,
            _mockSignInManager.Object
        );
    }

    #region AddUserAsync

    [TestMethod]
    public async Task AddUserAsync_CreatesUser()
    {
        var user = CreateUser(1);

        _mockUserManager
            .Setup(x => x.CreateAsync(user, It.IsAny<string>()))
            .ReturnsAsync(IdentityResult.Success);

        await _userService.AddUserAsync(user, "Password123!");

        Assert.AreNotEqual(Guid.Empty, user.RefreshToken);

        _mockUserManager.Verify(
            x => x.CreateAsync(user, "Password123!"),
            Times.Once
        );
    }

    [TestMethod]
    public async Task AddUserAsync_Throws_WhenCreateFails()
    {
        var user = CreateUser(1);

        _mockUserManager
            .Setup(x => x.CreateAsync(user, "bad"))
            .ReturnsAsync(IdentityResult.Failed(
                new IdentityError { Description = "fail" }
            ));

        await Assert.ThrowsExceptionAsync<InvalidDataException>(() =>
            _userService.AddUserAsync(user, "bad")
        );
    }

    #endregion

    #region LoginAsync

    [TestMethod]
    public async Task LoginAsync_Throws_WhenUserNotFound()
    {
        _mockUserManager
            .Setup(x => x.FindByEmailAsync("test@test.com"))
            .ReturnsAsync((AppUser?)null);

        await Assert.ThrowsExceptionAsync<AccessViolationException>(() =>
            _userService.LoginAsync("test@test.com", "123")
        );
    }

    [TestMethod]
    public async Task LoginAsync_Throws_WhenPasswordInvalid()
    {
        var user = CreateUser(1);

        _mockUserManager
            .Setup(x => x.FindByEmailAsync(user.Email!))
            .ReturnsAsync(user);

        _mockSignInManager
            .Setup(x => x.PasswordSignInAsync(user, "wrong", false, false))
            .ReturnsAsync(SignInResult.Failed);

        await Assert.ThrowsExceptionAsync<AccessViolationException>(() =>
            _userService.LoginAsync(user.Email!, "wrong")
        );
    }

    [TestMethod]
    public async Task LoginAsync_ReturnsTokens_WhenSuccess()
    {
        var user = CreateUser(1);
        user.RefreshToken = Guid.NewGuid();

        _mockUserManager
            .Setup(x => x.FindByEmailAsync(user.Email!))
            .ReturnsAsync(user);

        _mockSignInManager
            .Setup(x => x.PasswordSignInAsync(user, "123", false, false))
            .ReturnsAsync(SignInResult.Success);

        _mockUserManager
            .Setup(x => x.GetRolesAsync(user))
            .ReturnsAsync(new List<string>());

        var result = await _userService.LoginAsync(user.Email!, "123");

        Assert.IsFalse(string.IsNullOrEmpty(result.authToken));
        Assert.AreEqual(user.Id, result.userId);
    }

    #endregion

    #region GetCurrentUser

    [TestMethod]
    public async Task GetCurrentUserAsync_ReturnsNull_WhenNoUser()
    {
        _mockHttpContextAccessor
            .Setup(x => x.HttpContext)
            .Returns(new DefaultHttpContext());

        var result = await _userService.GetCurrentUserAsync();

        Assert.IsNull(result);
    }

    [TestMethod]
    public async Task GetCurrentUserAsync_ReturnsUser()
    {
        var user = CreateUser(1);

        SetCurrentUser("user1");

        _mockUserManager
            .Setup(x => x.FindByIdAsync("user1"))
            .ReturnsAsync(user);

        var result = await _userService.GetCurrentUserAsync();

        Assert.AreEqual("user1", result!.Id);
    }

    #endregion

    #region GetCurrentUserId

    [TestMethod]
    public void GetCurrentUserId_ReturnsNull_WhenNoContext()
    {
        _mockHttpContextAccessor
            .Setup(x => x.HttpContext)
            .Returns((HttpContext?)null);

        var result = _userService.GetCurrentUserId();

        Assert.IsNull(result);
    }

    [TestMethod]
    public void GetCurrentUserId_ReturnsId()
    {
        SetCurrentUser("user1");

        var result = _userService.GetCurrentUserId();

        Assert.AreEqual("user1", result);
    }

    #endregion

    #region Logout

    [TestMethod]
    public async Task LogoutAsync_DoesNothing_WhenNoUser()
    {
        _mockHttpContextAccessor
            .Setup(x => x.HttpContext)
            .Returns(new DefaultHttpContext());

        await _userService.LogoutAsync();

        _mockSignInManager.Verify(x => x.SignOutAsync(), Times.Never);
    }

    [TestMethod]
    public async Task LogoutAsync_CallsSignOut()
    {
        var user = CreateUser(1);

        SetCurrentUser("user1");

        _mockUserManager
            .Setup(x => x.FindByIdAsync("user1"))
            .ReturnsAsync(user);

        await _userService.LogoutAsync();

        _mockSignInManager.Verify(x => x.SignOutAsync(), Times.Once);
    }

    #endregion

    #region GetUserByIdAsync

    [TestMethod]
    public async Task GetUserByIdAsync_Throws_WhenNotSameUser()
    {
        var user = CreateUser(1);

        SetCurrentUser("user2");

        _mockUserManager
            .Setup(x => x.FindByIdAsync("user1"))
            .ReturnsAsync(user);

        await Assert.ThrowsExceptionAsync<AccessViolationException>(() =>
            _userService.GetUserByIdAsync("user1")
        );
    }

    [TestMethod]
    public async Task GetUserByIdAsync_ReturnsUser()
    {
        var user = CreateUser(1);

        SetCurrentUser("user1");

        _mockUserManager
            .Setup(x => x.FindByIdAsync("user1"))
            .ReturnsAsync(user);

        var result = await _userService.GetUserByIdAsync("user1");

        Assert.AreEqual("user1", result.Id);
    }

    #endregion

    #region Helpers

    private AppUser CreateUser(int i)
    {
        return new AppUser
        {
            Id = "user" + i,
            Email = $"user{i}@test.com",
            UserName = "user" + i,
            Name = "User " + i,
            RefreshToken = Guid.NewGuid()
        };
    }

    private void SetCurrentUser(string id)
    {
        var claims = new List<Claim> { new("id", id) };

        var identity = new ClaimsIdentity(claims, "test");
        var principal = new ClaimsPrincipal(identity);

        _mockHttpContextAccessor
            .Setup(x => x.HttpContext)
            .Returns(new DefaultHttpContext { User = principal });
    }

    private static Mock<UserManager<AppUser>> MockUserManager()
    {
        var store = new Mock<IUserStore<AppUser>>();

        return new Mock<UserManager<AppUser>>(
            store.Object, null!, null!, null!, null!, null!, null!, null!, null!
        );
    }

    private static Mock<SignInManager<AppUser>> MockSignInManager(UserManager<AppUser> userManager)
    {
        return new Mock<SignInManager<AppUser>>(
            userManager,
            new Mock<IHttpContextAccessor>().Object,
            new Mock<IUserClaimsPrincipalFactory<AppUser>>().Object,
            null!, null!, null!, null!
        );
    }

    #endregion
}