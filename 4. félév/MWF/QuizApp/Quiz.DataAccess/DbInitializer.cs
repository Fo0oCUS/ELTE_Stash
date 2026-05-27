using Microsoft.AspNetCore.Identity;
using Microsoft.EntityFrameworkCore;
using Quiz.DataAccess.Models;
using Quiz.DataAccess.Models.Enums;
using Quiz.DataAccess.Services;

namespace Quiz.DataAccess;

public class DbInitializer
{
    public static async Task Initialize(
        QuizAppDbContext context,
        RoleManager<UserRole>? roleManager = null,
        UserManager<AppUser>? userManager = null)
    {
        if (roleManager == null) throw new ArgumentNullException(nameof(roleManager));
        if (userManager == null) throw new ArgumentNullException(nameof(userManager));

        await SeedRolesAsync(roleManager);
        await SeedUsersAsync(userManager);

        var user1 = await userManager.FindByEmailAsync("user1@example.com");
        var user2 = await userManager.FindByEmailAsync("user2@example.com");

        if (user1 == null || user2 == null)
        {
            throw new Exception("Seed users were not created correctly.");
        }

        if (await context.Quizzes.AnyAsync())
        {
            return;
        }

        var quiz1 = new Quiz.DataAccess.Models.Quiz
        {
            Title = "Teszt Quiz",
            UserId = user1.Id,
            IsPublished = false,
            JoinCode = null,
            CurrentQuestionIndex = -1,
            Status = QuizStatus.WaitingToBePublished,
            Questions = new List<Question>
            {
                new()
                {
                    Title = "4 lába van asztal de nem szék?",
                    Answers = new List<string>
                    {
                        "Kecske",
                        "Attila"
                    },
                    IsOpen = true,
                    CorrectAnswerIndex = 1
                },
                new()
                {
                    Title = "Magyarország fővárosa",
                    Answers = new List<string>
                    {
                        "Albert",
                        "Közért",
                        "Budapest",
                        "Eszközért"
                    },
                    IsOpen = true,
                    CorrectAnswerIndex = 2
                }
            }
        };

        var quiz2 = new Quiz.DataAccess.Models.Quiz
        {
            Title = "Teszt Quiz2",
            UserId = user2.Id,
            IsPublished = true,
            JoinCode = "123456",
            CurrentQuestionIndex = -1,
            Status = QuizStatus.Lobby,
            Questions = new List<Question>
            {
                new()
                {
                    Title = "Piros?",
                    Answers = new List<string>
                    {
                        "Rozsaszín",
                        "Igen",
                        "Nem"
                    },
                    IsOpen = true,
                    CorrectAnswerIndex = 0
                }
            }
        };

        context.Quizzes.AddRange(quiz1, quiz2);
        await context.SaveChangesAsync();
    }

    private static async Task SeedRolesAsync(RoleManager<UserRole> roleManager)
    {
        string[] roleNames = { "Admin", "User" };

        foreach (var roleName in roleNames)
        {
            var roleExists = await roleManager.RoleExistsAsync(roleName);

            if (!roleExists)
            {
                await roleManager.CreateAsync(new UserRole(roleName));
            }
        }
    }

    private static async Task SeedUsersAsync(UserManager<AppUser> userManager)
    {
        const string adminEmail = "admin@example.com";
        const string adminPassword = "Admin123#";

        var adminUser = await userManager.FindByEmailAsync(adminEmail);
        if (adminUser == null)
        {
            adminUser = new AppUser
            {
                UserName = adminEmail,
                Email = adminEmail,
                Name = "Test Admin",
                RefreshToken = Guid.NewGuid()
            };

            await userManager.CreateAsync(adminUser, adminPassword);
            await userManager.AddToRoleAsync(adminUser, "Admin");
        }

        var users = new List<AppUser>
        {
            new()
            {
                UserName = "user1@example.com",
                Email = "user1@example.com",
                Name = "Test User1",
                RefreshToken = Guid.NewGuid()
            },
            new()
            {
                UserName = "user2@example.com",
                Email = "user2@example.com",
                Name = "Test User2",
                RefreshToken = Guid.NewGuid()
            },
            new()
            {
                UserName = "user3@example.com",
                Email = "user3@example.com",
                Name = "Test User3",
                RefreshToken = Guid.NewGuid()
            }
        };

        foreach (var user in users)
        {
            var existingUser = await userManager.FindByEmailAsync(user.Email!);
            if (existingUser == null)
            {
                await userManager.CreateAsync(user, "User123#");
                await userManager.AddToRoleAsync(user, "User");
            }
        }
    }
}