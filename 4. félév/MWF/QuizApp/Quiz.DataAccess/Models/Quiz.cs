
using System.ComponentModel.DataAnnotations;
using Quiz.DataAccess.Models.Enums;

namespace Quiz.DataAccess.Models;

public class Quiz
{
    [Required] public int Id { get; set; }

    [Length(1, 128)]
    public  required string Title { get; set; }
    public required string UserId { get; set; }
    public AppUser? User { get; set; }

    [Length(1, int.MaxValue)] public ICollection<Question> Questions { get; set; } = new List<Question>();

    public List<string> Messages { get; set; } = new List<string>();
    public int CurrentQuestionIndex { get; set; } = -1;

    public bool IsPublished { get; set; }
    public string? JoinCode { get; set; }

    public QuizStatus Status { get; set; } = QuizStatus.WaitingToBePublished;

    public List<string>? Players { get; set; }
}