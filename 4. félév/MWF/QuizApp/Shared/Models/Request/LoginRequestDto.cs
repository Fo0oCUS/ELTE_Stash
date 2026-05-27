using System.ComponentModel.DataAnnotations;

namespace Shared.Models.Request;

public record LoginRequestDto
{
    [EmailAddress(ErrorMessage = "Helytelen email cím.")]
    public required string Email { get; init; }
    
    public required string Password { get; init; }
}
