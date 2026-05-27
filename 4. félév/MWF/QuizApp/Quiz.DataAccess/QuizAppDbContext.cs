using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;
using Quiz.DataAccess.Models;

namespace Quiz.DataAccess;

public class QuizAppDbContext : IdentityDbContext<AppUser, UserRole, string>
{
    public DbSet<AppUser> AppUsers { get; set; } = null!;
    public DbSet<Question> Questions { get; set; } = null!;
    public DbSet<Models.Quiz> Quizzes { get; set; } = null!;
    
    public QuizAppDbContext(DbContextOptions<QuizAppDbContext> options) : base(options){}
    
    protected override void OnModelCreating(ModelBuilder builder)
    {
        base.OnModelCreating(builder);

        builder.Entity<Models.Quiz>().HasOne(x => x.User)
            .WithMany(x=>x.CreatedQuizzes)
            .HasForeignKey(x => x.UserId)
            .OnDelete(DeleteBehavior.Cascade);

        builder.Entity<Models.Quiz>().HasMany(x => x.Questions)
            .WithOne(x => x.Quiz)
            .HasForeignKey(x => x.QuizId)
            .OnDelete(DeleteBehavior.Cascade);
    }
}
