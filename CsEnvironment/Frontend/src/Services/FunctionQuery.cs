namespace MathEngine.src.Services;

using MathEngine.src.Models;

public class FunctionQuery
{
    public FunctionQuery(Function function)
    {
        this.function = function;
        // then we apply the algorithm
    }
    private Function function;
}