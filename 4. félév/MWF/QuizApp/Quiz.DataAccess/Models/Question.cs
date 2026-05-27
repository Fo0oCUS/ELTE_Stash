using System.ComponentModel.DataAnnotations;

namespace Quiz.DataAccess.Models;

public class Question
{
    [Required] public int Id { get; set; }

    [Required] public int QuizId { get; set; }
    public Quiz? Quiz { get; set; }

    public bool IsOpen { get; set; } = true;
    
    [Length(1, 512)]
    public required string Title { get; set; }

    [Length(2, int.MaxValue)] public List<string> Answers { get; set; } = new();

    public int CorrectAnswerIndex { get; set; }
}