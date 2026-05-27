namespace Shared.Models.Responses;

public class QuestionResponseDto
{
    public required string Title { get; set; }
    public required List<string> Answers { get; set; }
    public int CorrectAnswerIndex { get; set; }
    public bool IsOpen { get; set; }
}