using System.ComponentModel.DataAnnotations;

namespace Shared.Models.Request;

public class AppUserRequestDto
{
    [StringLength(255, ErrorMessage = "Name is too long")]
    public required string Name { get; init; }
    
    [EmailAddress(ErrorMessage = "Email is invalid")]
    public required string Email { get; init; }
    
    public required string Password { get; init; }
}