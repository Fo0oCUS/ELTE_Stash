namespace Shared.Models.Request;

public class QuizMessageRequestDto
{
    public required string JoinCode { get; set; }
    public required string PlayerName { get; set; }
    public required string Message { get; set; }
}