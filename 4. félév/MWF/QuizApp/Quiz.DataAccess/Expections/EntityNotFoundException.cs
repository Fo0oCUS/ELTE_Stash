namespace Quiz.DataAccess.Services.Expections;

public class EntityNotFoundException : Exception
{
    public EntityNotFoundException(string name): base($"{name} entity not found.") {}
}