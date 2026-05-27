using AutoMapper;

namespace QuizApp.WebApi.Infrastructure;

public static class DependencyInjection
{
    public static IServiceCollection AddAutomapper(this IServiceCollection services)
    {
        var mapperConfig = new MapperConfiguration(cfg => cfg.AddProfile(new MappingProfile()));
        mapperConfig.AssertConfigurationIsValid();

        services.AddAutoMapper(typeof(MappingProfile));

        return services;
    }
}
