using System.Text.RegularExpressions;
using Microsoft.AspNetCore.SignalR;

namespace QuizApp.WebApi.Hubs;

public class QuizHub : Hub
{
    public async Task JoinQuizGroup(string joinCode)
    {
        await Groups.AddToGroupAsync(Context.ConnectionId, GetQuizGroupName(joinCode));
    }

    public async Task LeaveQuizGroup(string joinCode)
    {
        await Groups.RemoveFromGroupAsync(Context.ConnectionId, GetQuizGroupName(joinCode));
    }
    public static string GetQuizGroupName(string joinCode)
    {
        return $"quiz-{joinCode}";
    }
}