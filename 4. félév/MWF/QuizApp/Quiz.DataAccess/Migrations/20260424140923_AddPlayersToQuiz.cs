using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Quiz.DataAccess.Migrations
{
    /// <inheritdoc />
    public partial class AddPlayersToQuiz : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "Players",
                table: "Quizzes",
                type: "nvarchar(max)",
                nullable: true);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Players",
                table: "Quizzes");
        }
    }
}
