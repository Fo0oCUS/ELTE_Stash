using System.Net;
using System.Net.Http.Headers;
using System.Net.Http.Json;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc.Testing;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;
using Quiz.DataAccess;
using Quiz.DataAccess.Models;
using Shared.Models.Request;
using Shared.Models.Responses;

namespace Quiz.Test.IntegrationTests;

[TestClass]
public class UsersControllerIntegrationTests
{
    private HttpClient _client = null!;
    private WebApplicationFactory<Program> _factory = null!;

    private static readonly LoginRequestDto ExistingUserLogin = new()
    {
        Email = "user@example.com",
        Password = "User123#"
    };

    [TestInitialize]
    public void Init()
    {
        Environment.SetEnvironmentVariable("ASPNETCORE_ENVIRONMENT", "IntegrationTest");

        var databaseName = Guid.NewGuid().ToString();

        _factory = new WebApplicationFactory<Program>()
            .WithWebHostBuilder(builder =>
            {
                builder.ConfigureServices(services =>
                {
                    var descriptor = services.SingleOrDefault(
                        d => d.ServiceType == typeof(DbContextOptions<QuizAppDbContext>));

                    if (descriptor != null)
                        services.Remove(descriptor);

                    services.AddDbContext<QuizAppDbContext>(options =>
                    {
                        options.UseInMemoryDatabase(databaseName);
                    });

                    using var scope = services.BuildServiceProvider().CreateScope();
                    var serviceProvider = scope.ServiceProvider;

                    var db = serviceProvider.GetRequiredService<QuizAppDbContext>();
                    db.Database.EnsureDeleted();
                    db.Database.EnsureCreated();

                    var roleManager = serviceProvider.GetRequiredService<RoleManager<UserRole>>();
                    SeedRoles(roleManager);

                    var userManager = serviceProvider.GetRequiredService<UserManager<AppUser>>();
                    SeedUsers(userManager);
                });
            });

        _client = _factory.CreateClient();
    }

    #region Create

    [TestMethod]
    public async Task CreateUser_CreatesUser_WhenDataIsValid()
    {
        var request = new AppUserRequestDto
        {
            Name = "NewUser",
            Email = "newuser@example.com",
            Password = "NewUser123#"
        };

        var response = await _client.PostAsJsonAsync("/users", request);

        Assert.AreEqual(HttpStatusCode.Created, response.StatusCode);

        var user = await response.Content.ReadFromJsonAsync<AppUserResponseDto>();

        Assert.IsNotNull(user);
        Assert.AreEqual("NewUser", user.Name);
        Assert.AreEqual("newuser@example.com", user.Email);
    }

    [TestMethod]
    public async Task CreateUser_ReturnsBadRequest_WhenDataIsInvalid()
    {
        var request = new AppUserRequestDto
        {
            Name = "",
            Email = "not-an-email",
            Password = "123"
        };

        var response = await _client.PostAsJsonAsync("/users", request);

        Assert.AreEqual(HttpStatusCode.BadRequest, response.StatusCode);
    }

    #endregion

    #region Login

    [TestMethod]
    public async Task Login_ReturnsOk_WhenCredentialsAreValid()
    {
        var response = await _client.PostAsJsonAsync("/users/login", ExistingUserLogin);

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var loginResponse = await response.Content.ReadFromJsonAsync<LoginResponseDto>();

        Assert.IsNotNull(loginResponse);
        Assert.IsFalse(string.IsNullOrWhiteSpace(loginResponse.AuthToken));
        Assert.IsFalse(string.IsNullOrWhiteSpace(loginResponse.RefreshToken));
        Assert.IsFalse(string.IsNullOrWhiteSpace(loginResponse.UserId));
    }

    [TestMethod]
    public async Task Login_ReturnsForbidden_WhenPasswordIsInvalid()
    {
        var request = new LoginRequestDto
        {
            Email = ExistingUserLogin.Email,
            Password = "WrongPassword123#"
        };

        var response = await _client.PostAsJsonAsync("/users/login", request);

        Assert.AreEqual(HttpStatusCode.Forbidden, response.StatusCode);
    }

    #endregion

    #region Logout

    [TestMethod]
    public async Task Logout_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var response = await _client.PostAsync("/users/logout", null);

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task Logout_ReturnsNoContent_WhenLoggedIn()
    {
        await Login();

        var response = await _client.PostAsync("/users/logout", null);

        Assert.AreEqual(HttpStatusCode.NoContent, response.StatusCode);
    }

    #endregion

    #region Refresh

    [TestMethod]
    public async Task Refresh_ReturnsOk_WhenRefreshTokenIsValid()
    {
        var loginResponse = await Login();

        var response = await _client.PostAsJsonAsync("/users/refresh", loginResponse.RefreshToken);

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var refreshResponse = await response.Content.ReadFromJsonAsync<LoginResponseDto>();

        Assert.IsNotNull(refreshResponse);
        Assert.IsFalse(string.IsNullOrWhiteSpace(refreshResponse.AuthToken));
        Assert.AreEqual(loginResponse.RefreshToken, refreshResponse.RefreshToken);
        Assert.AreEqual(loginResponse.UserId, refreshResponse.UserId);
    }

    [TestMethod]
    public async Task Refresh_ReturnsForbidden_WhenRefreshTokenIsInvalid()
    {
        var response = await _client.PostAsJsonAsync("/users/refresh", "not-valid-token");

        Assert.AreEqual(HttpStatusCode.Forbidden, response.StatusCode);
    }

    #endregion

    #region GetUser

    [TestMethod]
    public async Task GetUser_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var response = await _client.GetAsync("/users/some-user-id");

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task GetUser_ReturnsUser_WhenLoggedInAndSameUser()
    {
        var loginResponse = await Login();

        var response = await _client.GetAsync($"/users/{loginResponse.UserId}");

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var user = await response.Content.ReadFromJsonAsync<AppUserResponseDto>();

        Assert.IsNotNull(user);
        Assert.AreEqual(ExistingUserLogin.Email, user.Email);
    }

    #endregion

    #region Helpers

    private static void SeedRoles(RoleManager<UserRole> roleManager)
    {
        var roleNames = new[] { "User" };

        foreach (var roleName in roleNames)
        {
            var roleExists = roleManager.RoleExistsAsync(roleName).Result;

            if (!roleExists)
            {
                roleManager.CreateAsync(new UserRole(roleName)).Wait();
            }
        }
    }

    private static void SeedUsers(UserManager<AppUser> userManager)
    {
        var user = userManager.FindByEmailAsync(ExistingUserLogin.Email).Result;

        if (user == null)
        {
            user = new AppUser
            {
                UserName = ExistingUserLogin.Email,
                Email = ExistingUserLogin.Email,
                Name = "Test User",
                RefreshToken = Guid.NewGuid()
            };

            userManager.CreateAsync(user, ExistingUserLogin.Password).Wait();
            userManager.AddToRoleAsync(user, "User").Wait();
        }
    }

    private async Task<LoginResponseDto> Login()
    {
        var response = await _client.PostAsJsonAsync("/users/login", ExistingUserLogin);

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var loginResponse = await response.Content.ReadFromJsonAsync<LoginResponseDto>();

        Assert.IsNotNull(loginResponse);
        Assert.IsFalse(string.IsNullOrWhiteSpace(loginResponse.AuthToken));

        _client.DefaultRequestHeaders.Authorization =
            new AuthenticationHeaderValue("Bearer", loginResponse.AuthToken);

        return loginResponse;
    }

    [TestCleanup]
    public void CleanUp()
    {
        using var scope = _factory.Services.CreateScope();

        var db = scope.ServiceProvider.GetRequiredService<QuizAppDbContext>();
        db.Database.EnsureDeleted();

        _client.Dispose();
        _factory.Dispose();
    }

    #endregion
}