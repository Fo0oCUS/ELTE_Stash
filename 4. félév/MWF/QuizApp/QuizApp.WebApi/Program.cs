

using System.Reflection;
using System.Text;
using System.Text.Json.Serialization;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.IdentityModel.Tokens;
using Microsoft.OpenApi.Models;
using Quiz.DataAccess;
using Quiz.DataAccess.Models;
using Quiz.DataAccess.Services;
using Quiz.DataAccess.Services.Config;
using QuizApp.WebApi.Hubs;
using QuizApp.WebApi.Infrastructure;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.

builder.Services.AddControllers(options =>
    {
        options.Filters.Add(new ProducesAttribute("application/json"));

    })
    .AddJsonOptions(options =>
    {
        options.JsonSerializerOptions.ReferenceHandler = ReferenceHandler.IgnoreCycles;
        options.JsonSerializerOptions.Converters.Add(new JsonStringEnumConverter());
    });

// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();

builder.Services.AddSwaggerGen(c =>
{
    c.SwaggerDoc("v1", new OpenApiInfo
    {
        Title = "Quiz API",
        Version = "v1",
        Description = "MWF Beadando Quiz API"
    });
    c.AddSecurityDefinition("Bearer", new OpenApiSecurityScheme()
    {
        Name = "Authorization",
        Type = SecuritySchemeType.ApiKey,
        Scheme = "Bearer",
        BearerFormat = "JWT",
        In = ParameterLocation.Header,
        Description = "JWT Authorization header using the Bearer scheme. \r\n\r\n Enter 'Bearer' [space] and then your token in the text input below.\r\n\r\nExample: \"Bearer 1safsfsdfdfd\"",
    });
    c.AddSecurityRequirement(new OpenApiSecurityRequirement {
        {
            new OpenApiSecurityScheme {
                Reference = new OpenApiReference {
                    Type = ReferenceType.SecurityScheme,
                    Id = "Bearer"
                }
            },
            Array.Empty<string>()
        }
    });
});

builder.Services.AddDataAccess(builder.Configuration);
builder.Services.AddAuthorization();

var jwtSection = builder.Configuration.GetSection("JwtSettings");
var jwtSettings = jwtSection.Get<JwtSettings>() ?? throw new ArgumentNullException(nameof(JwtSettings));
builder.Services.Configure<JwtSettings>(jwtSection);

builder.Services.AddAuthentication(options =>
{
    options.DefaultAuthenticateScheme = JwtBearerDefaults.AuthenticationScheme;
    options.DefaultChallengeScheme = JwtBearerDefaults.AuthenticationScheme;
    options.DefaultScheme = JwtBearerDefaults.AuthenticationScheme;
}).AddJwtBearer(options =>
{
    options.TokenValidationParameters = new TokenValidationParameters()
    {
        ValidAudience = jwtSettings.Audience,
        ValidIssuer = jwtSettings.Issuer,
        ClockSkew = TimeSpan.Zero,
        IssuerSigningKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(jwtSettings.SecretKey))
    };
});


builder.Services.AddAutomapper();
builder.Services.AddSignalR();
builder.Services.AddProblemDetails();
builder.Services.AddExceptionHandler<ExceptionToProblemDetailsHandler>();

var app = builder.Build();

app.UseExceptionHandler();
app.UseStatusCodePages();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment()) {
    app.UseSwagger();
    app.UseSwaggerUI();
}


app.UseHttpsRedirection();

app.UseAuthentication();
app.UseAuthorization();

app.MapControllers();

app.MapHub<QuizHub>("/hubs/quiz");

var host = Host.CreateDefaultBuilder(args)
    .ConfigureAppConfiguration((_, config) => {
        config.AddJsonFile("appsettings.json", optional: false, reloadOnChange: true);
        config.AddUserSecrets<Program>();
    })
    .ConfigureServices((hostContext, services) => {
        services.AddDataAccess(hostContext.Configuration);
    }).Build();

await host.StartAsync();
            
using (var scope = host.Services.CreateScope())
{
    var context = scope.ServiceProvider.GetRequiredService<QuizAppDbContext>();
                
    var roleManager = scope.ServiceProvider.GetRequiredService<RoleManager<UserRole>>();
    var userManager = scope.ServiceProvider.GetRequiredService<UserManager<AppUser>>();
                
    await DbInitializer.Initialize(context, roleManager, userManager);
}


await app.RunAsync();
