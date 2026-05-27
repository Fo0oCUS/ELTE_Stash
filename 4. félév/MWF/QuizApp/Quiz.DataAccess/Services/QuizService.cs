using System.Globalization;
using Microsoft.EntityFrameworkCore;
using Quiz.DataAccess.Models.Enums;
using Quiz.DataAccess.Services.Expections;
using Quiz.DataAccess.Services.Interfaces;

namespace Quiz.DataAccess.Services;

using Models;

public class QuizService : IQuizService
{
    private readonly QuizAppDbContext _context;
    private readonly IUserService _userService;
    
    public QuizService(QuizAppDbContext context, IUserService userService)
    {
        _context = context;
        _userService = userService;
    }

    public async Task<IReadOnlyCollection<Quiz>> GetByUserIdAsync(string id)
    {
        var user = await _userService.GetCurrentUserAsync();

        if (user == null || user.Id != id)
        {
            throw new AccessViolationException("User not accessible");
        }
        
        return await _context.Quizzes
            .Include(x => x.Questions)
            .Where(x => x.UserId == id)
            .OrderByDescending(x => x.Id)
            .ToListAsync();
    }

    public async Task<Quiz> GetByIdAsync(int id)
    {
        var user = await _userService.GetCurrentUserAsync();
        var quiz = await _context.Quizzes
            .Include(x=>x.Questions)
            .FirstOrDefaultAsync(x => x.Id == id);

        if (quiz == null) throw new EntityNotFoundException("Quiz not found");
        if (user == null || user.Id != quiz.UserId)
        {
            throw new AccessViolationException("You don't have rights to view this quiz");
        }

        return quiz;
    }
    
    public async Task<Quiz> AddAsync(Quiz quiz)
    {
        var user = await _userService.GetCurrentUserAsync();
        if(user == null || quiz.UserId != user.Id) throw new AccessViolationException("You can't create quizzes in other's name.");

        await _context.Quizzes.AddAsync(quiz);
        
        await _context.SaveChangesAsync();
        
        return quiz;
    }

    public async Task<Quiz> UpdateAsync(Quiz newQuiz)
    {
        var quiz = await _context.Quizzes
            .Include(q => q.Questions)
            .FirstOrDefaultAsync(q => q.Id == newQuiz.Id);

        if (quiz == null)
            throw new EntityNotFoundException("Quiz not found.");

        if (quiz.IsPublished)
            throw new Exception("Quiz can not be updated while published.");

        var user = await _userService.GetCurrentUserAsync();
        if (user == null || quiz.UserId != user.Id)
            throw new AccessViolationException("You don't have rights to update this quiz");

        quiz.Title = newQuiz.Title;

        quiz.Questions.Clear();

        foreach (var q in newQuiz.Questions)
        {
            quiz.Questions.Add(new Question
            {
                Title = q.Title,
                Answers = q.Answers,
                CorrectAnswerIndex = q.CorrectAnswerIndex
            });
        }

        await _context.SaveChangesAsync();

        return quiz;
    }

    public async Task<Quiz> PublishQuizAsync(int id)
    {
        var quiz = await GetByIdAsync(id);

        Random r = new Random();
        int joinCode;
        do
        {
            joinCode = r.Next(100000, 1000000);

        } while (await _context.Quizzes.AnyAsync(x => x.JoinCode == joinCode.ToString()));
        
        quiz.IsPublished = true;
        quiz.JoinCode = joinCode.ToString();
        quiz.Status = QuizStatus.Lobby;
        quiz.Players = new List<string>();
        
        await _context.SaveChangesAsync();
        return quiz;
    }

    public async Task<Quiz> GetQuizByJoinCode(string player, string joinCode)
    {
        var quiz = await _context.Quizzes.Include(x => x.Questions)
            .FirstOrDefaultAsync(x=>x.JoinCode == joinCode);
        if (quiz == null) throw new EntityNotFoundException("Quiz not found.");
        if (!quiz.IsPublished) throw new Exception("The quiz is not published yet.");
        if (!quiz.Players!.Contains(player)) throw new Exception("You have to join the quiz first.");
        
        return quiz;
    }

    public async Task EndQuizAsync(int quizId)
    {
        var quiz = await GetByIdAsync(quizId);
        if (!quiz.IsPublished) throw new Exception("The quiz is not published yet.");
        
        var user = await _userService.GetCurrentUserAsync();
        if(user == null || quiz.UserId != user.Id) throw new AccessViolationException("You don't have rights to end this quiz");
        
        quiz.CurrentQuestionIndex = -1;
        quiz.IsPublished = false;
        quiz.Status = QuizStatus.WaitingToBePublished;
        quiz.JoinCode = null;
        quiz.Players = null;
        quiz.Messages = new List<string>();
        
        foreach (var question in quiz.Questions) { question.IsOpen = true; }
        
        await _context.SaveChangesAsync();
    }

    public async Task<Quiz> NextQuestionAsync(int quizId)
    {
        var quiz = await GetByIdAsync(quizId);
        if (!quiz.IsPublished) throw new Exception("The quiz is not published yet.");
        if (quiz.Status == QuizStatus.Finished) throw new Exception("The quiz already finished.");
        
        var user = await _userService.GetCurrentUserAsync();
        if(user == null || quiz.UserId != user.Id) throw new AccessViolationException("You don't have rights to go to the next question in this quiz");
        
        quiz.CurrentQuestionIndex++;
        if (quiz.CurrentQuestionIndex == 0){
            quiz.Status = QuizStatus.InProgress;
        }
        
        await _context.SaveChangesAsync();
        return quiz;
    }

    public async Task JoinQuizAsync(string joinCode, string player)
    {
        var quiz = await _context.Quizzes.Include(x => x.Questions)
            .FirstOrDefaultAsync(x=>x.JoinCode == joinCode);
        if (quiz == null) throw new EntityNotFoundException("Quiz not found.");
        if (!quiz.IsPublished) throw new Exception("The quiz is not published yet.");
        if (quiz.Status == QuizStatus.Finished) throw new Exception("The quiz already finished.");

        if (quiz.JoinCode != joinCode) throw new Exception("You can not you this quiz with the given join code ");
        if (quiz.Players!.Contains(player)) throw new Exception("Username already taken.");

        quiz.Players.Add(player);
        await _context.SaveChangesAsync();
    }

    public async Task CloseCurrentQuestion(int quizId)
    {
        var quiz = await GetByIdAsync(quizId);
        
        quiz.Questions.ElementAt(quiz.CurrentQuestionIndex).IsOpen = false;
        
        await _context.SaveChangesAsync();
    }


    public async Task<string> SendMessageByJoinCodeAsync(string player, string joinCode, string message)
    {
        var quiz = await GetQuizByJoinCode(player, joinCode);
        
        quiz.Messages.Add(player + ": " + message);

        await _context.SaveChangesAsync();
        return quiz.Messages.Last();
    }
}
