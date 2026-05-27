using System.Net;
using System.Net.Http.Headers;
using System.Net.Http.Json;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc.Testing;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;
using Quiz.DataAccess;
using Quiz.DataAccess.Models;
using Quiz.DataAccess.Models.Enums;
using Shared.Models.Request;
using Shared.Models.Responses;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Quiz.Test.IntegrationTests;

[TestClass]
public class QuizControllerIntegrationTests
{
    private HttpClient _client = null!;
    private WebApplicationFactory<Program> _factory = null!;

    private static readonly LoginRequestDto UserLogin = new()
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
                    var userId = SeedUsers(userManager);

                    SeedQuizzes(db, userId);
                });
            });

        _client = _factory.CreateClient();
    }

    #region Get

    [TestMethod]
    public async Task GetQuizById_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var response = await _client.GetAsync("/quizzes/1");

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task GetQuizById_ReturnsQuiz_WhenQuizExistsAndUserIsOwner()
    {
        await Login();

        var response = await _client.GetAsync("/quizzes/1");

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var quiz = await response.Content.ReadFromJsonAsync<QuizResponseDto>(JsonOptions);

        Assert.IsNotNull(quiz);
        Assert.AreEqual(1, quiz.Id);
        Assert.AreEqual("Test Quiz 1", quiz.Title);
    }

    [TestMethod]
    public async Task GetQuizById_ReturnsNotFound_WhenQuizDoesntExist()
    {
        await Login();

        var response = await _client.GetAsync("/quizzes/99");

        Assert.AreEqual(HttpStatusCode.NotFound, response.StatusCode);
    }

    [TestMethod]
    public async Task GetMyQuizzes_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var response = await _client.GetAsync("/quizzes/mine");

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task GetMyQuizzes_ReturnsCurrentUsersQuizzes()
    {
        await Login();

        var response = await _client.GetAsync("/quizzes/mine");

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var quizzes = await response.Content.ReadFromJsonAsync<List<QuizResponseDto>>(JsonOptions);

        Assert.IsNotNull(quizzes);
        Assert.IsTrue(quizzes.Count >= 1);
        Assert.IsTrue(quizzes.Any(q => q.Title == "Test Quiz 1"));
    }

    #endregion

    #region Create

    [TestMethod]
    public async Task CreateQuiz_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var request = CreateValidQuizRequest("New Quiz");

        var response = await _client.PostAsJsonAsync("/quizzes", request);

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task CreateQuiz_CreatesQuiz_WhenLoggedIn()
    {
        await Login();

        var request = CreateValidQuizRequest("New Quiz");

        var response = await _client.PostAsJsonAsync("/quizzes", request);

        Assert.AreEqual(HttpStatusCode.Created, response.StatusCode);

        var createdQuiz = await response.Content.ReadFromJsonAsync<QuizResponseDto>(JsonOptions);

        Assert.IsNotNull(createdQuiz);
        Assert.AreEqual("New Quiz", createdQuiz.Title);
    }

    #endregion

    #region Update

    [TestMethod]
    public async Task UpdateQuiz_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var request = CreateValidQuizRequest("Updated Quiz");

        var response = await _client.PostAsJsonAsync("/quizzes/1/update", request);

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task UpdateQuiz_UpdatesQuiz_WhenUserIsOwner()
    {
        await Login();

        var request = CreateValidQuizRequest("Updated Quiz");

        var response = await _client.PostAsJsonAsync("/quizzes/1/update", request);

        Assert.AreEqual(HttpStatusCode.Accepted, response.StatusCode);

        var updatedQuiz = await response.Content.ReadFromJsonAsync<QuizResponseDto>(JsonOptions);

        Assert.IsNotNull(updatedQuiz);
        Assert.AreEqual("Updated Quiz", updatedQuiz.Title);
    }

    [TestMethod]
    public async Task UpdateQuiz_ReturnsNotFound_WhenQuizDoesntExist()
    {
        await Login();

        var request = CreateValidQuizRequest("Updated Quiz");

        var response = await _client.PostAsJsonAsync("/quizzes/99/update", request);

        Assert.AreEqual(HttpStatusCode.NotFound, response.StatusCode);
    }

    #endregion

    #region Publish

    [TestMethod]
    public async Task PublishQuiz_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var response = await _client.GetAsync("/quizzes/publish/1");

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task PublishQuiz_PublishesQuiz_WhenUserIsOwner()
    {
        await Login();

        var response = await _client.GetAsync("/quizzes/publish/1");

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var quiz = await response.Content.ReadFromJsonAsync<QuizResponseDto>(JsonOptions);

        Assert.IsNotNull(quiz);
        Assert.IsTrue(quiz.IsPublished);
        Assert.IsFalse(string.IsNullOrWhiteSpace(quiz.JoinCode));
    }

    #endregion

    #region Join

    [TestMethod]
    public async Task JoinQuiz_ReturnsNotFound_WhenJoinCodeDoesntExist()
    {
        var request = new JoinRequestDto
        {
            JoinCode = "999999",
            PlayerName = "player1"
        };

        var response = await _client.PostAsJsonAsync("/quizzes/join", request);

        Assert.AreEqual(HttpStatusCode.NotFound, response.StatusCode);
    }

    [TestMethod]
    public async Task JoinQuiz_JoinsQuiz_WhenJoinCodeIsValid()
    {
        var request = new JoinRequestDto
        {
            JoinCode = "123456",
            PlayerName = "player1"
        };

        var response = await _client.PostAsJsonAsync("/quizzes/join", request);

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);
    }

    [TestMethod]
    public async Task GetQuizByJoinCode_ReturnsQuiz_WhenPlayerAlreadyJoined()
    {
        var joinRequest = new JoinRequestDto
        {
            JoinCode = "123456",
            PlayerName = "player1"
        };

        await _client.PostAsJsonAsync("/quizzes/join", joinRequest);

        var request = new GetQuizByJoinCodeRequestDto
        {
            JoinCode = "123456",
            PlayerName = "player1"
        };

        var response = await _client.PostAsJsonAsync("/quizzes/code", request);

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var quiz = await response.Content.ReadFromJsonAsync<QuizResponseForPlayerDto>(JsonOptions);

        Assert.IsNotNull(quiz);
        Assert.AreEqual("Test Published Quiz", quiz.Title);
    }

    #endregion

    #region GameFlow

    [TestMethod]
    public async Task NextQuestion_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var response = await _client.GetAsync("/quizzes/2/next");

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task NextQuestion_GoesToNextQuestion_WhenUserIsOwner()
    {
        await Login();

        var response = await _client.GetAsync("/quizzes/2/next");

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var quiz = await response.Content.ReadFromJsonAsync<QuizResponseForPlayerDto>(JsonOptions);

        Assert.IsNotNull(quiz);
        Assert.AreEqual(0, quiz.CurrentQuestionIndex);
    }

    [TestMethod]
    public async Task CloseCurrentQuestion_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var response = await _client.GetAsync("/quizzes/2/question/end");

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task CloseCurrentQuestion_ClosesQuestion_WhenUserIsOwner()
    {
        await Login();

        await _client.GetAsync("/quizzes/2/next");

        var response = await _client.GetAsync("/quizzes/2/question/end");

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);
    }

    [TestMethod]
    public async Task EndQuiz_ReturnsUnauthorized_WhenNotLoggedIn()
    {
        var response = await _client.GetAsync("/quizzes/2/end");

        Assert.AreEqual(HttpStatusCode.Unauthorized, response.StatusCode);
    }

    [TestMethod]
    public async Task EndQuiz_EndsQuiz_WhenUserIsOwner()
    {
        await Login();

        var response = await _client.GetAsync("/quizzes/2/end");

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);
    }

    #endregion

    #region Message

    [TestMethod]
    public async Task SendMessage_ReturnsOk_WhenPlayerJoined()
    {
        var joinRequest = new JoinRequestDto
        {
            JoinCode = "123456",
            PlayerName = "player1"
        };

        await _client.PostAsJsonAsync("/quizzes/join", joinRequest);

        var messageRequest = new QuizMessageRequestDto
        {
            JoinCode = "123456",
            PlayerName = "player1",
            Message = "hello"
        };

        var response = await _client.PostAsJsonAsync("/quizzes/message", messageRequest);

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);
    }

    #endregion

    #region Helpers

    private static QuizRequestDto CreateValidQuizRequest(string title)
    {
        return new QuizRequestDto
        {
            Title = title,
            Questions = new List<QuestionRequestDto>
            {
                new QuestionRequestDto
                {
                    Title = "Question 1",
                    Answers = new List<string>
                    {
                        "Answer 1",
                        "Answer 2",
                        "Answer 3",
                        "Answer 4"
                    },
                    CorrectAnswerIndex = 0
                }
            }
        };
    }

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

    private static string SeedUsers(UserManager<AppUser> userManager)
    {
        var user = userManager.FindByEmailAsync(UserLogin.Email).Result;

        if (user == null)
        {
            user = new AppUser
            {
                UserName = UserLogin.Email,
                Email = UserLogin.Email,
                Name = "Test User",
                RefreshToken = Guid.NewGuid()
            };

            userManager.CreateAsync(user, UserLogin.Password).Wait();
            userManager.AddToRoleAsync(user, "User").Wait();
        }

        return user.Id;
    }

    private static void SeedQuizzes(QuizAppDbContext context, string userId)
    {
        context.Quizzes.AddRange(
            new Quiz.DataAccess.Models.Quiz
            {
                Id = 1,
                Title = "Test Quiz 1",
                UserId = userId,
                IsPublished = false,
                Status = QuizStatus.WaitingToBePublished,
                CurrentQuestionIndex = -1,
                Questions = new List<Question>
                {
                    new Question
                    {
                        Title = "Question 1",
                        Answers = new List<string> { "A", "B", "C", "D" },
                        CorrectAnswerIndex = 0,
                        IsOpen = true
                    }
                },
                Messages = new List<string>()
            },
            new Quiz.DataAccess.Models.Quiz
            {
                Id = 2,
                Title = "Test Published Quiz",
                UserId = userId,
                IsPublished = true,
                JoinCode = "123456",
                Status = QuizStatus.Lobby,
                CurrentQuestionIndex = -1,
                Players = new List<string>(),
                Messages = new List<string>(),
                Questions = new List<Question>
                {
                    new Question
                    {
                        Title = "Question 1",
                        Answers = new List<string> { "A", "B", "C", "D" },
                        CorrectAnswerIndex = 1,
                        IsOpen = true
                    }
                }
            }
        );

        context.SaveChanges();
    }

    private async Task Login()
    {
        var response = await _client.PostAsJsonAsync("/users/login", UserLogin);

        Assert.AreEqual(HttpStatusCode.OK, response.StatusCode);

        var loginResponse = await response.Content.ReadFromJsonAsync<LoginResponseDto>();

        Assert.IsNotNull(loginResponse);
        Assert.IsFalse(string.IsNullOrWhiteSpace(loginResponse.AuthToken));

        _client.DefaultRequestHeaders.Authorization =
            new AuthenticationHeaderValue("Bearer", loginResponse.AuthToken);
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
    
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
        Converters = { new JsonStringEnumConverter() }
    };
}

