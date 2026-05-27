namespace Shared.Models.Responses;


public record LoginResponseDto
{
    public required string UserId { get; init; }

    public required string AuthToken { get; init; }
    
    public required string RefreshToken { get; init; }
}
