using Microsoft.AspNetCore.Diagnostics;
using Microsoft.AspNetCore.Mvc;
using Quiz.DataAccess.Services.Expections;

namespace QuizApp.WebApi.Infrastructure;

public class ExceptionToProblemDetailsHandler : IExceptionHandler
{
    private readonly IProblemDetailsService _problemDetailsService;

    public ExceptionToProblemDetailsHandler(IProblemDetailsService problemDetailsService)
    {
        _problemDetailsService = problemDetailsService;
    }
    
    public async ValueTask<bool> TryHandleAsync(HttpContext httpContext, Exception exception, CancellationToken cancellationToken)
    {
        return exception switch
        {
            EntityNotFoundException => await CreateProblemDetails(httpContext, exception,
                StatusCodes.Status404NotFound),
            UnauthorizedAccessException => await CreateProblemDetails(httpContext, exception,
                StatusCodes.Status403Forbidden),
            AccessViolationException => await CreateProblemDetails(httpContext, exception,
                StatusCodes.Status403Forbidden),
            ArgumentOutOfRangeException => await CreateProblemDetails(httpContext, exception,
                StatusCodes.Status400BadRequest),
            ArgumentNullException => await CreateProblemDetails(httpContext, exception,
                StatusCodes.Status400BadRequest),
            ArgumentException => await CreateProblemDetails(httpContext, exception,
                StatusCodes.Status409Conflict),
            InvalidDataException => await CreateProblemDetails(httpContext, exception,
                StatusCodes.Status409Conflict),
            InvalidOperationException => await CreateProblemDetails(httpContext, exception,
                StatusCodes.Status409Conflict),
            _ => await CreateProblemDetails(httpContext, exception,
                StatusCodes.Status500InternalServerError)
        };
    }

    private async Task<bool> CreateProblemDetails(HttpContext httpContext, Exception exception, int statusCode)
    {
        httpContext.Response.StatusCode = statusCode;

        var problemDetails = new ProblemDetails
        {
            Title = "An error occurred",
            Type = exception.GetType().Name,
            Detail = exception.Message,
        };

        return await _problemDetailsService.TryWriteAsync(new ProblemDetailsContext
        {
            Exception = exception,
            HttpContext = httpContext,
            ProblemDetails = problemDetails
        });
    }
}
