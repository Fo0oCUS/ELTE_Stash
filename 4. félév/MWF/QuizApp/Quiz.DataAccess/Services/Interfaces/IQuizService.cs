using Quiz.DataAccess.Models;

namespace Quiz.DataAccess.Services.Interfaces;

public interface IQuizService
{
    public Task<IReadOnlyCollection<Models.Quiz>> GetByUserIdAsync(string id);

    public Task<Models.Quiz> GetByIdAsync(int id);
    public Task<Models.Quiz> AddAsync(Models.Quiz quiz);
    public Task<Models.Quiz> UpdateAsync(Models.Quiz quiz);
    public Task<Models.Quiz> PublishQuizAsync(int id);
    public Task<Models.Quiz> GetQuizByJoinCode(string player, string joinCode);
    public Task EndQuizAsync(int quizId);
    public Task<Models.Quiz> NextQuestionAsync(int quizId);
    public Task JoinQuizAsync(string joinCode, string player);
    public Task CloseCurrentQuestion(int quizId);
    public Task<string> SendMessageByJoinCodeAsync(string player, string joinCode, string message);
}