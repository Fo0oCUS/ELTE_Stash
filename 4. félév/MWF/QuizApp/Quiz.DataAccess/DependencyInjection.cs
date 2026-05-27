using Microsoft.AspNetCore.Identity;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Quiz.DataAccess.Models;
using Quiz.DataAccess.Services;
using Quiz.DataAccess.Services.Interfaces;

namespace Quiz.DataAccess;

public static class DependencyInjection
{
    public static IServiceCollection AddDataAccess(this IServiceCollection services, IConfiguration config)
    {   
        var connectionString = config.GetConnectionString("DefaultConnection");
        services.AddDbContext<QuizAppDbContext>(options => options
            .UseSqlServer(connectionString)
        );
        
        services.AddIdentity<AppUser, UserRole>(options =>
            {
                // Password settings.
                options.Password.RequiredLength = 6;
                
                // Lockout settings.
                options.Lockout.DefaultLockoutTimeSpan = TimeSpan.FromMinutes(5);
                options.Lockout.MaxFailedAccessAttempts = 5;
                options.Lockout.AllowedForNewUsers = true;
                options.User.RequireUniqueEmail = true;
            })
            .AddEntityFrameworkStores<QuizAppDbContext>()
            .AddDefaultTokenProviders();
        services.AddHttpContextAccessor();
        services.AddScoped<IUserService, UserService>();
        services.AddScoped<IQuizService, QuizService>();
        
        return services;
    }
}
