using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Quiz.DataAccess.Migrations
{
    /// <inheritdoc />
    public partial class AddMessagesToQuiz : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "Messages",
                table: "Quizzes",
                type: "nvarchar(max)",
                nullable: false,
                defaultValue: "[]");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Messages",
                table: "Quizzes");
        }
    }
}
