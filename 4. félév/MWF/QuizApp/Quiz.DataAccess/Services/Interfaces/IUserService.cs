using Quiz.DataAccess.Models;
using Quiz.DataAccess.Models.Enums;

namespace Quiz.DataAccess.Services.Interfaces;

public interface IUserService
{
    Task AddUserAsync(AppUser user, string password);
    Task<(string authToken, string refreshToken, string userId)> LoginAsync(string email, string password);
    Task<(string authToken, string refreshToken, string userId)> RedeemRefreshTokenAsync(string refreshToken);
    Task LogoutAsync();
    Task<AppUser?> GetCurrentUserAsync();
    string? GetCurrentUserId();
    public Task<AppUser> GetUserByIdAsync(string id);
}
