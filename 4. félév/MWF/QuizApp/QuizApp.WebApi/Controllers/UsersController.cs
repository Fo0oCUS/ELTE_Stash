using System.ComponentModel.DataAnnotations;
using AutoMapper;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Quiz.DataAccess.Models;
using Quiz.DataAccess.Services.Interfaces;
using Shared.Models.Request;
using Shared.Models.Responses;

namespace QuizApp.WebApi.Controllers;


[ApiController]
[Route("/users")]
public class UsersController : ControllerBase
{
    private readonly IUserService _usersService;
    private readonly IMapper _mapper;

    public UsersController(IMapper mapper, IUserService usersService)
    {
        _mapper = mapper;
        _usersService = usersService;
    }
    
    [HttpPost]
    [ProducesResponseType(statusCode: StatusCodes.Status201Created, type: typeof(AppUserResponseDto))]
    public async Task<IActionResult> CreateUser([FromBody] AppUserRequestDto userRequestDto)
    {
        var user = _mapper.Map<AppUser>(userRequestDto);

        await _usersService.AddUserAsync(user, userRequestDto.Password);

        var userResponseDto = _mapper.Map<AppUserResponseDto>(user);

        return StatusCode(StatusCodes.Status201Created, userResponseDto);
    }

    [HttpPost]
    [Route("login")]
    [ProducesResponseType(statusCode: StatusCodes.Status200OK, type: typeof(AppUserResponseDto))]
    public async Task<IActionResult> Login([FromBody] LoginRequestDto loginRequestDto)
    {
        var (authToken, refreshToken, userId) = await _usersService.LoginAsync(loginRequestDto.Email, loginRequestDto.Password);

        var loginResponseDto = new LoginResponseDto
        {
            UserId = userId,
            AuthToken = authToken,
            RefreshToken = refreshToken,
        };

        return Ok(loginResponseDto);
    }

    [HttpPost]
    [Route("logout")]
    [Authorize]
    [ProducesResponseType(statusCode: StatusCodes.Status204NoContent, type: typeof(void))]
    public async Task<IActionResult> Logout()
    {
        await _usersService.LogoutAsync();

        return NoContent();
    }

    [HttpPost]
    [Route("refresh")]
    [ProducesResponseType(statusCode: StatusCodes.Status200OK, type: typeof(AppUserResponseDto))]
    public async Task<IActionResult> RedeemRefreshToken([FromBody] string refreshToken)
    {
        var (authToken, newRefreshToken, userId) = await _usersService.RedeemRefreshTokenAsync(refreshToken);

        var loginResponseDto = new LoginResponseDto
        {
            UserId = userId,
            AuthToken = authToken,
            RefreshToken = newRefreshToken,
        };

        return Ok(loginResponseDto);
    }
    
    [HttpGet]
    [Route("{id}")]
    [Authorize]
    [ProducesResponseType(statusCode: StatusCodes.Status200OK, type: typeof(AppUserResponseDto))]
    public async Task<IActionResult> GetUser([FromRoute][Required] string id)
    {
        var user = await _usersService.GetUserByIdAsync(id);
        var userResponseDto = _mapper.Map<AppUserResponseDto>(user);

        return Ok(userResponseDto);
    }
}
