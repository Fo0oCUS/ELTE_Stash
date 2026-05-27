using System.ComponentModel.DataAnnotations;

namespace Shared.Models.Request;

public class QuestionRequestDto
{
    [Required]
    public required string Title { get; set; }
    
    [Required]
    [Length(2, int.MaxValue)]
    public required List<string> Answers { get; set; }
    
    public int CorrectAnswerIndex { get; set; }
}