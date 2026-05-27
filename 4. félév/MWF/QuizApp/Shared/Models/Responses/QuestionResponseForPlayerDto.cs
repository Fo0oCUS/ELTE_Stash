namespace Shared.Models.Responses;

public class QuestionResponseForPlayerDto
{
    public required string Title { get; set; }
    public required List<string> Answers { get; set; }
    public bool IsOpen { get; set; }
}