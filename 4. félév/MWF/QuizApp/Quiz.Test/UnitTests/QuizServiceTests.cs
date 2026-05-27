using Microsoft.EntityFrameworkCore;
using Moq;
using Quiz.DataAccess;
using Quiz.DataAccess.Models;
using Quiz.DataAccess.Models.Enums;
using Quiz.DataAccess.Services;
using Quiz.DataAccess.Services.Expections;
using Quiz.DataAccess.Services.Interfaces;

namespace Quiz.Test.UnitTests;

[TestClass]
public class QuizServiceTests : IDisposable, IAsyncDisposable
{
    private QuizAppDbContext _context = null!;
    private QuizService _quizService = null!;
    private Mock<IUserService> _mockUserService = null!;

    [TestInitialize]
    public void Init()
    {
        var options = new DbContextOptionsBuilder<QuizAppDbContext>()
            .UseInMemoryDatabase(Guid.NewGuid().ToString())
            .Options;

        _context = new QuizAppDbContext(options);

        _mockUserService = new Mock<IUserService>();

        _quizService = new QuizService(_context, _mockUserService.Object);
    }

    #region GetByIdAsync

    [TestMethod]
    public async Task GetByIdAsync_ThrowsEntityNotFoundException_WhenQuizDoesntExist()
    {
        await Assert.ThrowsExceptionAsync<EntityNotFoundException>(() =>
            _quizService.GetByIdAsync(12)
        );
    }

    [TestMethod]
    public async Task GetByIdAsync_ThrowsAccessViolationException_WhenUserIsNotCreator()
    {
        var quiz = CreateQuiz(1, 1);
        var user2 = CreateAppUser(2);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user2);

        await Assert.ThrowsExceptionAsync<AccessViolationException>(() =>
            _quizService.GetByIdAsync(1)
        );
    }

    [TestMethod]
    public async Task GetByIdAsync_ReturnsQuiz_WhenUserIsCreator()
    {
        var quiz = CreateQuiz(1, 1);
        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        var result = await _quizService.GetByIdAsync(1);

        Assert.AreEqual(1, result.Id);
        Assert.AreEqual("quiz1", result.Title);
        Assert.AreEqual("user1", result.UserId);
    }

    #endregion

    #region GetByUserIdAsync

    [TestMethod]
    public async Task GetByUserIdAsync_ThrowsAccessViolationException_WhenUserIsNotSameAsRequestedUser()
    {
        var user2 = CreateAppUser(2);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user2);

        await Assert.ThrowsExceptionAsync<AccessViolationException>(() =>
            _quizService.GetByUserIdAsync("user1")
        );
    }

    [TestMethod]
    public async Task GetByUserIdAsync_ReturnsUsersQuizzes()
    {
        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(CreateQuiz(1, 1));
        await AddQuizToDbAsync(CreateQuiz(2, 1));
        await AddQuizToDbAsync(CreateQuiz(3, 2));

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        var result = await _quizService.GetByUserIdAsync("user1");

        Assert.AreEqual(2, result.Count);
        Assert.IsTrue(result.All(x => x.UserId == "user1"));
    }

    #endregion

    #region AddAsync

    [TestMethod]
    public async Task AddAsync_ThrowsAccessViolationException_WhenUserTriesToCreateQuizInOthersName()
    {
        var quiz = CreateQuiz(1, 1);
        var user2 = CreateAppUser(2);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user2);

        await Assert.ThrowsExceptionAsync<AccessViolationException>(() =>
            _quizService.AddAsync(quiz)
        );
    }

    [TestMethod]
    public async Task AddAsync_AddsQuiz()
    {
        var user1 = CreateAppUser(1);
        var quiz = CreateQuiz(1, 1);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        Assert.AreEqual(0, await _context.Quizzes.CountAsync());

        await _quizService.AddAsync(quiz);

        Assert.AreEqual(1, await _context.Quizzes.CountAsync());
    }

    [TestMethod]
    public async Task AddAsync_ReturnsCreatedQuiz()
    {
        var user1 = CreateAppUser(1);
        var quiz = CreateQuiz(1, 1);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        var result = await _quizService.AddAsync(quiz);

        Assert.AreEqual(1, result.Id);
        Assert.AreEqual("quiz1", result.Title);
        Assert.AreEqual("user1", result.UserId);
    }

    #endregion

    #region UpdateAsync

    [TestMethod]
    public async Task UpdateAsync_ThrowsEntityNotFoundException_WhenQuizDoesntExist()
    {
        var newQuiz = CreateQuiz(1, 1);

        await Assert.ThrowsExceptionAsync<EntityNotFoundException>(() =>
            _quizService.UpdateAsync(newQuiz)
        );
    }

    [TestMethod]
    public async Task UpdateAsync_ThrowsException_WhenQuizIsPublished()
    {
        var quiz = CreateQuiz(1, 1);
        quiz.IsPublished = true;

        var newQuiz = CreateQuiz(1, 1);
        newQuiz.Title = "updated quiz";

        await AddQuizToDbAsync(quiz);

        await Assert.ThrowsExceptionAsync<Exception>(() =>
            _quizService.UpdateAsync(newQuiz)
        );
    }

    [TestMethod]
    public async Task UpdateAsync_ThrowsAccessViolationException_WhenUserIsNotCreator()
    {
        var quiz = CreateQuiz(1, 1);
        var newQuiz = CreateQuiz(1, 1);
        var user2 = CreateAppUser(2);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user2);

        await Assert.ThrowsExceptionAsync<AccessViolationException>(() =>
            _quizService.UpdateAsync(newQuiz)
        );
    }

    [TestMethod]
    public async Task UpdateAsync_UpdatesQuizTitle()
    {
        var quiz = CreateQuiz(1, 1);
        var user1 = CreateAppUser(1);

        var newQuiz = CreateQuiz(1, 1);
        newQuiz.Title = "updated quiz";

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        var result = await _quizService.UpdateAsync(newQuiz);

        Assert.AreEqual("updated quiz", result.Title);
    }

    [TestMethod]
    public async Task UpdateAsync_ReplacesQuestions()
    {
        var quiz = CreateQuizWithQuestions(1, 1, 2);
        var user1 = CreateAppUser(1);

        var newQuiz = CreateQuizWithQuestions(1, 1, 1);
        newQuiz.Questions.First().Title = "new question";

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        var result = await _quizService.UpdateAsync(newQuiz);

        Assert.AreEqual(1, result.Questions.Count);
        Assert.AreEqual("new question", result.Questions.First().Title);
    }

    #endregion

    #region PublishQuizAsync

    [TestMethod]
    public async Task PublishQuizAsync_PublishesQuiz()
    {
        var quiz = CreateQuiz(1, 1);
        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        var result = await _quizService.PublishQuizAsync(1);

        Assert.IsTrue(result.IsPublished);
        Assert.AreEqual(QuizStatus.Lobby, result.Status);
        Assert.IsNotNull(result.JoinCode);
        Assert.AreEqual(6, result.JoinCode.Length);
        Assert.IsNotNull(result.Players);
        Assert.AreEqual(0, result.Players.Count);
    }

    [TestMethod]
    public async Task PublishQuizAsync_ThrowsAccessViolationException_WhenUserIsNotCreator()
    {
        var quiz = CreateQuiz(1, 1);
        var user2 = CreateAppUser(2);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user2);

        await Assert.ThrowsExceptionAsync<AccessViolationException>(() =>
            _quizService.PublishQuizAsync(1)
        );
    }

    #endregion

    #region JoinQuizAsync

    [TestMethod]
    public async Task JoinQuizAsync_ThrowsEntityNotFoundException_WhenQuizDoesntExist()
    {
        await Assert.ThrowsExceptionAsync<EntityNotFoundException>(() =>
            _quizService.JoinQuizAsync("123456", "player1")
        );
    }

    [TestMethod]
    public async Task JoinQuizAsync_ThrowsException_WhenQuizIsNotPublished()
    {
        var quiz = CreateQuiz(1, 1);
        quiz.JoinCode = "123456";
        quiz.IsPublished = false;
        quiz.Players = new List<string>();

        await AddQuizToDbAsync(quiz);

        await Assert.ThrowsExceptionAsync<Exception>(() =>
            _quizService.JoinQuizAsync("123456", "player1")
        );
    }

    [TestMethod]
    public async Task JoinQuizAsync_ThrowsException_WhenQuizAlreadyFinished()
    {
        var quiz = CreatePublishedQuiz(1, 1);
        quiz.Status = QuizStatus.Finished;

        await AddQuizToDbAsync(quiz);

        await Assert.ThrowsExceptionAsync<Exception>(() =>
            _quizService.JoinQuizAsync("123456", "player1")
        );
    }

    [TestMethod]
    public async Task JoinQuizAsync_ThrowsException_WhenPlayerNameAlreadyTaken()
    {
        var quiz = CreatePublishedQuiz(1, 1);
        quiz.Players!.Add("player1");

        await AddQuizToDbAsync(quiz);

        await Assert.ThrowsExceptionAsync<Exception>(() =>
            _quizService.JoinQuizAsync("123456", "player1")
        );
    }

    [TestMethod]
    public async Task JoinQuizAsync_AddsPlayer()
    {
        var quiz = CreatePublishedQuiz(1, 1);

        await AddQuizToDbAsync(quiz);

        await _quizService.JoinQuizAsync("123456", "player1");

        var result = await _context.Quizzes.FirstAsync(x => x.Id == 1);

        Assert.AreEqual(1, result.Players!.Count);
        Assert.IsTrue(result.Players.Contains("player1"));
    }

    #endregion

    #region GetQuizByJoinCode

    [TestMethod]
    public async Task GetQuizByJoinCode_ThrowsEntityNotFoundException_WhenQuizDoesntExist()
    {
        await Assert.ThrowsExceptionAsync<EntityNotFoundException>(() =>
            _quizService.GetQuizByJoinCode("player1", "123456")
        );
    }

    [TestMethod]
    public async Task GetQuizByJoinCode_ThrowsException_WhenQuizIsNotPublished()
    {
        var quiz = CreateQuiz(1, 1);
        quiz.JoinCode = "123456";
        quiz.IsPublished = false;
        quiz.Players = new List<string> { "player1" };

        await AddQuizToDbAsync(quiz);

        await Assert.ThrowsExceptionAsync<Exception>(() =>
            _quizService.GetQuizByJoinCode("player1", "123456")
        );
    }

    [TestMethod]
    public async Task GetQuizByJoinCode_ThrowsException_WhenPlayerDidNotJoin()
    {
        var quiz = CreatePublishedQuiz(1, 1);

        await AddQuizToDbAsync(quiz);

        await Assert.ThrowsExceptionAsync<Exception>(() =>
            _quizService.GetQuizByJoinCode("player1", "123456")
        );
    }

    [TestMethod]
    public async Task GetQuizByJoinCode_ReturnsQuiz_WhenPlayerAlreadyJoined()
    {
        var quiz = CreatePublishedQuiz(1, 1);
        quiz.Players!.Add("player1");

        await AddQuizToDbAsync(quiz);

        var result = await _quizService.GetQuizByJoinCode("player1", "123456");

        Assert.AreEqual(1, result.Id);
        Assert.AreEqual("123456", result.JoinCode);
    }

    #endregion

    #region NextQuestionAsync

    [TestMethod]
    public async Task NextQuestionAsync_ThrowsException_WhenQuizIsNotPublished()
    {
        var quiz = CreateQuiz(1, 1);
        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        await Assert.ThrowsExceptionAsync<Exception>(() =>
            _quizService.NextQuestionAsync(1)
        );
    }

    [TestMethod]
    public async Task NextQuestionAsync_ThrowsException_WhenQuizAlreadyFinished()
    {
        var quiz = CreatePublishedQuiz(1, 1);
        quiz.Status = QuizStatus.Finished;

        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        await Assert.ThrowsExceptionAsync<Exception>(() =>
            _quizService.NextQuestionAsync(1)
        );
    }

    [TestMethod]
    public async Task NextQuestionAsync_IncreasesCurrentQuestionIndex()
    {
        var quiz = CreatePublishedQuiz(1, 1);
        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        var result = await _quizService.NextQuestionAsync(1);

        Assert.AreEqual(0, result.CurrentQuestionIndex);
    }

    [TestMethod]
    public async Task NextQuestionAsync_SetsStatusToInProgress_WhenFirstQuestionStarts()
    {
        var quiz = CreatePublishedQuiz(1, 1);
        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        var result = await _quizService.NextQuestionAsync(1);

        Assert.AreEqual(QuizStatus.InProgress, result.Status);
    }

    #endregion

    #region CloseCurrentQuestion

    [TestMethod]
    public async Task CloseCurrentQuestion_ClosesCurrentQuestion()
    {
        var quiz = CreatePublishedQuizWithQuestions(1, 1, 2);
        quiz.CurrentQuestionIndex = 0;

        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        await _quizService.CloseCurrentQuestion(1);

        var result = await _context.Quizzes
            .Include(x => x.Questions)
            .FirstAsync(x => x.Id == 1);

        Assert.IsFalse(result.Questions.ElementAt(0).IsOpen);
    }

    #endregion

    #region SendMessageByJoinCodeAsync

    [TestMethod]
    public async Task SendMessageByJoinCodeAsync_AddsMessage()
    {
        var quiz = CreatePublishedQuiz(1, 1);
        quiz.Players!.Add("player1");
        quiz.Messages = new List<string>();

        await AddQuizToDbAsync(quiz);

        var result = await _quizService.SendMessageByJoinCodeAsync(
            "player1",
            "123456",
            "hello"
        );

        Assert.AreEqual("player1: hello", result);

        var savedQuiz = await _context.Quizzes.FirstAsync(x => x.Id == 1);

        Assert.AreEqual(1, savedQuiz.Messages.Count);
        Assert.AreEqual("player1: hello", savedQuiz.Messages.Last());
    }

    #endregion

    #region EndQuizAsync

    [TestMethod]
    public async Task EndQuizAsync_ThrowsException_WhenQuizIsNotPublished()
    {
        var quiz = CreateQuiz(1, 1);
        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        await Assert.ThrowsExceptionAsync<Exception>(() =>
            _quizService.EndQuizAsync(1)
        );
    }

    [TestMethod]
    public async Task EndQuizAsync_EndsQuiz()
    {
        var quiz = CreatePublishedQuizWithQuestions(1, 1, 2);
        quiz.CurrentQuestionIndex = 1;
        quiz.Messages = new List<string> { "player1: hello" };

        quiz.Questions.ElementAt(0).IsOpen = false;
        quiz.Questions.ElementAt(1).IsOpen = false;

        var user1 = CreateAppUser(1);

        await AddQuizToDbAsync(quiz);

        _mockUserService
            .Setup(x => x.GetCurrentUserAsync())
            .ReturnsAsync(user1);

        await _quizService.EndQuizAsync(1);

        var result = await _context.Quizzes
            .Include(x => x.Questions)
            .FirstAsync(x => x.Id == 1);

        Assert.AreEqual(-1, result.CurrentQuestionIndex);
        Assert.IsFalse(result.IsPublished);
        Assert.AreEqual(QuizStatus.WaitingToBePublished, result.Status);
        Assert.IsNull(result.JoinCode);
        Assert.IsNull(result.Players);
        Assert.AreEqual(0, result.Messages.Count);
        Assert.IsTrue(result.Questions.All(x => x.IsOpen));
    }

    #endregion

    #region Helpers

    private async Task AddQuizToDbAsync(Quiz.DataAccess.Models.Quiz quiz)
    {
        _context.Quizzes.Add(quiz);
        await _context.SaveChangesAsync();
    }

    private AppUser CreateAppUser(int index)
    {
        return new AppUser
        {
            UserName = "user" + index,
            Id = "user" + index,
            Email = "user" + index + "@example.com"
        };
    }

    private Quiz.DataAccess.Models.Quiz CreateQuiz(int quizIndex, int userIndex)
    {
        return new Quiz.DataAccess.Models.Quiz
        {
            Id = quizIndex,
            Title = "quiz" + quizIndex,
            UserId = "user" + userIndex,
            IsPublished = false,
            Status = QuizStatus.WaitingToBePublished,
            CurrentQuestionIndex = -1,
            Questions = new List<Question>(),
            Messages = new List<string>()
        };
    }

    private Quiz.DataAccess.Models.Quiz CreateQuizWithQuestions(
        int quizIndex,
        int userIndex,
        int questionCount)
    {
        var quiz = CreateQuiz(quizIndex, userIndex);

        for (var i = 1; i <= questionCount; i++)
        {
            quiz.Questions.Add(CreateQuestion(i));
        }

        return quiz;
    }

    private Quiz.DataAccess.Models.Quiz CreatePublishedQuiz(int quizIndex, int userIndex)
    {
        var quiz = CreateQuiz(quizIndex, userIndex);

        quiz.IsPublished = true;
        quiz.JoinCode = "123456";
        quiz.Status = QuizStatus.Lobby;
        quiz.Players = new List<string>();
        quiz.Messages = new List<string>();

        return quiz;
    }

    private Quiz.DataAccess.Models.Quiz CreatePublishedQuizWithQuestions(
        int quizIndex,
        int userIndex,
        int questionCount)
    {
        var quiz = CreateQuizWithQuestions(quizIndex, userIndex, questionCount);

        quiz.IsPublished = true;
        quiz.JoinCode = "123456";
        quiz.Status = QuizStatus.Lobby;
        quiz.Players = new List<string>();
        quiz.Messages = new List<string>();

        return quiz;
    }

    private Question CreateQuestion(int index)
    {
        return new Question
        {
            Id = index,
            Title = "question" + index,
            Answers = new List<string>
            {
                "answer1",
                "answer2",
                "answer3",
                "answer4"
            },
            CorrectAnswerIndex = 0,
            IsOpen = true
        };
    }

    #endregion

    [TestCleanup]
    public void CleanUp()
    {
        Dispose();
    }

    public void Dispose()
    {
        _context.Database.EnsureDeleted();
        _context.Dispose();
    }

    public async ValueTask DisposeAsync()
    {
        await _context.DisposeAsync();
    }
}