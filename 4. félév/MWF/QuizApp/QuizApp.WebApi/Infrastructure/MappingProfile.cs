using AutoMapper;
using Quiz.DataAccess.Models;
using Shared.Models.Request;
using Shared.Models.Responses;

namespace QuizApp.WebApi.Infrastructure;

public class MappingProfile : Profile
{
    
    public MappingProfile()
    {
        // REQUEST DTO -> ENTITY

        CreateMap<AppUserRequestDto, AppUser>(MemberList.Source)
            .ForSourceMember(src => src.Password, opt => opt.DoNotValidate())
            .ForMember(dest => dest.UserName, opt => opt.MapFrom(x=>x.Name));

        CreateMap<QuizRequestDto, Quiz.DataAccess.Models.Quiz>(MemberList.Source);
        
        CreateMap<QuestionRequestDto, Question>(MemberList.Source);
        
            
        
        // ENTITY -> RESPONSE DTO

        CreateMap<AppUser, AppUserResponseDto>(MemberList.Destination);

        CreateMap<Quiz.DataAccess.Models.Quiz, QuizResponseDto>(MemberList.Destination);
        CreateMap<Quiz.DataAccess.Models.Quiz, QuizResponseForPlayerDto>(MemberList.Destination);
        
        CreateMap<Question, QuestionResponseDto>(MemberList.Destination);
        CreateMap<Question, QuestionResponseForPlayerDto>(MemberList.Destination);



    }
}
