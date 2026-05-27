using System.ComponentModel.DataAnnotations;

namespace Shared.Models.Request;

public class QuizRequestDto
{
    [Length(1, 128)]
    public  required string Title { get; set; }
    [Length(1, int.MaxValue)]
    public required ICollection<QuestionRequestDto> Questions { get; set; }
}