using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Quiz.DataAccess.Migrations
{
    /// <inheritdoc />
    public partial class AddJoinCodeToQuiz : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "JoinCode",
                table: "Quizzes",
                type: "nvarchar(max)",
                nullable: true);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "JoinCode",
                table: "Quizzes");
        }
    }
}
