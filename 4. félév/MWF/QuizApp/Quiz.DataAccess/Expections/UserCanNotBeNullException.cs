namespace Quiz.DataAccess.Services.Expections;

public class UserCanNotBeNullException : Exception
{
    public UserCanNotBeNullException() : base("A felhasználó nem létezik."){}
}