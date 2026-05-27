namespace Shared.Models.Responses;

public class AppUserResponseDto
{ 
    public required string Id { get; init; }
    
    public required string Name { get; init; }
    
    public required string Email { get; init; }
    
    public  required List<QuizResponseDto> CreatedQuizzes { get; init; }
}