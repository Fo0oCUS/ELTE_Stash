using System.ComponentModel.DataAnnotations;
using Microsoft.AspNetCore.Identity;

namespace Quiz.DataAccess.Models;

public class AppUser : IdentityUser
{
    [MaxLength(20)]
    [MinLength(3)]
    public string Name { get; set; } = string.Empty;
    
    public Guid? RefreshToken { get; set; }
    
    public List<Quiz> CreatedQuizzes { get; set; } = new List<Quiz>();
}
