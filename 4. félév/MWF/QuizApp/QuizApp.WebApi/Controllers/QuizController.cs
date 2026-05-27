using AutoMapper;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.SignalR;
using Quiz.DataAccess.Services.Interfaces;
using QuizApp.WebApi.Hubs;
using Shared.Models.Request;
using Shared.Models.Responses;

namespace QuizApp.WebApi.Controllers;

[ApiController]
[Route("/quizzes")]
public class QuizController : ControllerBase
{
    private readonly IQuizService _quizService;
    private readonly IMapper _mapper;
    private readonly IUserService _userService;
    private readonly IHubContext<QuizHub> _hubContext;

    public QuizController(IQuizService quizService, IMapper mapper,
        IUserService userService, IHubContext<QuizHub> hubContext)
    {
        _quizService = quizService;
        _mapper = mapper;
        _userService = userService;
        _hubContext = hubContext;
    }

    [HttpPost]
    [Authorize]
    [ProducesResponseType(statusCode: StatusCodes.Status201Created, Type = typeof(QuizResponseDto))]
    public async Task<IActionResult> CreateQuiz([FromBody] QuizRequestDto quizRequestDto)
    {
        var quiz = _mapper.Map<Quiz.DataAccess.Models.Quiz>(quizRequestDto);
        var user = await _userService.GetCurrentUserAsync();
        quiz.UserId = user!.Id;
        
        var createdQuiz = await _quizService.AddAsync(quiz);
    
        var response = _mapper.Map<QuizResponseDto>(createdQuiz);
    
        return CreatedAtAction(nameof(CreateQuiz), new { quizId = response.Id }, response);
    }

    [HttpPost("{id}/update")]
    [Authorize]
    [ProducesResponseType(statusCode: StatusCodes.Status202Accepted, Type = typeof(QuizResponseDto))]
    public async Task<IActionResult> UpdateQuiz([FromRoute] int id, [FromBody] QuizRequestDto quizRequestDto)
    {
        var newQuiz = _mapper.Map<Quiz.DataAccess.Models.Quiz>(quizRequestDto);
        newQuiz.Id = id;
        var quiz = await _quizService.UpdateAsync(newQuiz);
        var response = _mapper.Map<QuizResponseDto>(quiz);
        return Accepted(response);
    }
    
    [HttpGet("{quizId}")]
    [Authorize]
    [ProducesResponseType(statusCode: StatusCodes.Status200OK, type: typeof(QuizResponseDto))]
    public async Task<IActionResult> GetQuizById([FromRoute] int quizId)
    {
        var quiz = await _quizService.GetByIdAsync(quizId);
    
        var response = _mapper.Map<QuizResponseDto>(quiz);
        return Ok(response);
    }
    
    [HttpGet("mine")]
    [Authorize]
    [ProducesResponseType(statusCode: StatusCodes.Status200OK, type: typeof(List<QuizResponseDto>))]
    public async Task<IActionResult> GetMyQuizzes()
    {
        var userId = _userService.GetCurrentUserId() ?? throw new UnauthorizedAccessException("User is not authenticated.");
        var quizzes = await _quizService.GetByUserIdAsync(userId);
    
        var response = _mapper.Map<List<QuizResponseDto>>(quizzes);
        return Ok(response);
    }

    [HttpGet("publish/{id}")]
    [Authorize]
    [ProducesResponseType(statusCode: StatusCodes.Status201Created, type: typeof(QuizResponseDto))]
    public async Task<IActionResult> PublishQuiz([FromRoute] int id)
    {
        var quiz = await _quizService.PublishQuizAsync(id);
        var response = _mapper.Map<QuizResponseDto>(quiz);
        return Ok(response);
    }

    [HttpPost("join")]
    public async Task<IActionResult> JoinQuiz([FromBody] JoinRequestDto joinRequestDto)
    {
        await _quizService.JoinQuizAsync(joinRequestDto.JoinCode, joinRequestDto.PlayerName);
        return Ok();
    }

    [HttpPost("code")]
    public async Task<IActionResult> GetQuizByJoinCode([FromBody] GetQuizByJoinCodeRequestDto dto)
    {
        var quiz = await _quizService.GetQuizByJoinCode(dto.PlayerName, dto.JoinCode);
        var response = _mapper.Map<QuizResponseForPlayerDto>(quiz);
        return Ok(response);
    }

    [HttpGet("{id}/next")]
    [Authorize]
    public async Task<IActionResult> NextQuestion([FromRoute] int id)
    {
        var quiz = await _quizService.NextQuestionAsync(id);
        var response = _mapper.Map<QuizResponseForPlayerDto>(quiz);
        
        await _hubContext.Clients.Group(QuizHub.GetQuizGroupName(response.JoinCode!))
            .SendAsync("QuestionChanged");
        
        return Ok(response);
    }
    
    [HttpGet("{id}/end")]
    [Authorize]
    public async Task<IActionResult> EndQuiz([FromRoute] int id)
    {
        var quiz = await _quizService.GetByIdAsync(id);
        var joinCode = quiz.JoinCode!;
        
        await _quizService.EndQuizAsync(id);

        await _hubContext.Clients.Group(QuizHub.GetQuizGroupName(joinCode))
            .SendAsync("QuizEnded");
        
        return Ok();
    }
    
    [HttpGet("{id}/question/end")]
    [Authorize]
    public async Task<IActionResult> CloseCurrentQuestion([FromRoute] int id)
    {
        await _quizService.CloseCurrentQuestion(id);
        
        var quiz = await _quizService.GetByIdAsync(id);
        var correctAnswerIndex = quiz.Questions.ElementAt(quiz.CurrentQuestionIndex).CorrectAnswerIndex;
        await _hubContext.Clients.Group(QuizHub.GetQuizGroupName(quiz.JoinCode!))
            .SendAsync("QuestionClosed", correctAnswerIndex);
            
        return Ok();
    }

    [HttpPost("message")]
    public async Task<IActionResult> SendMessage([FromBody] QuizMessageRequestDto dto)
    {
        var message = await _quizService.SendMessageByJoinCodeAsync(dto.PlayerName, dto.JoinCode, dto.Message);

        await _hubContext.Clients.Groups(QuizHub.GetQuizGroupName(dto.JoinCode))
            .SendAsync("MessageSent", message);

        return Ok();
    }
    
    
}
