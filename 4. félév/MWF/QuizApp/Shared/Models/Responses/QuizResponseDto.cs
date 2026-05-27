using Quiz.DataAccess.Models;
using Quiz.DataAccess.Models.Enums;

namespace Shared.Models.Responses;

public class QuizResponseDto
{
    public int Id { get; set; }
    public required string Title { get; set; }
    public required ICollection<QuestionResponseDto> Questions { get; set; }
    public int CurrentQuestionIndex { get; set; }
    public required List<string> Messages { get; set; }
    public bool IsPublished { get; set; }
    public required string JoinCode { get; set; }

    public QuizStatus Status { get; set; }
}