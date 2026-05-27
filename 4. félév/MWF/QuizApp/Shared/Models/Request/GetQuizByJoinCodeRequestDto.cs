namespace Shared.Models.Request;

public class GetQuizByJoinCodeRequestDto
{
    public required string PlayerName { get; set; }
    public required string JoinCode { get; set; }
}