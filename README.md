# MathAnalysisCalculator

MathAnalysisCalculator is an early-stage C++ mathematics engine for calculations from mathematical analysis. The long-term goal is to analyze an implicit equation

```text
F(x, y, z) = 0
```

and locate and classify local extrema using first- and second-order derivatives (the Jacobian/gradient and Hessian). At present, the repository contains foundational expression parsing and symbolic differentiation code, plus a separate integer-matrix determinant program. It does **not** yet find extrema or calculate Jacobians or Hessians.

## Project status

| Area | Current status |
| --- | --- |
| Expression input | A single expression can be read by the analysis executable. |
| Parsing | Builds an owned expression tree for the supported syntax below. |
| Symbolic differentiation | Can construct first partial derivatives with respect to `x`, `y`, or `z` for supported expressions. |
| Derivative output | Prints the derivative as a parenthesized expression string. |
| Expression evaluation | Not implemented. |
| Simplification | Not implemented; `Simplifier` is an empty placeholder. |
| Jacobian, Hessian, extrema analysis | Not implemented. |
| Matrix determinant | Separate executable for square integer matrices. |

This status describes the code currently in the repository; the future-work sections are design direction, not implemented behavior.

## Repository layout

```text
.
├── README.md
└── CppEnvironment/
    ├── CMakeLists.txt
    ├── derivativesEngine/
    │   ├── ComputeFunction.cpp       # Analysis executable entry point
    │   ├── DifferentiationEngine.h   # Recursive symbolic differentiation
    │   ├── ExpressionHandler.h       # Parse, differentiate, and print facade
    │   ├── FunctionDataStructure.h   # Expression-tree node and node types
    │   ├── FunctionParser.h          # Recursive-descent parser
    │   ├── FunctionUnparser.h        # AST-to-expression printer
    │   └── Simplifier.h              # Empty placeholder
    └── resources/
        └── MatrixDetCalc.cpp         # Standalone integer determinant program
```

## Requirements and build

- CMake 3.25 or newer
- A C++ compiler with C++23 support

Configure and build from the repository root:

```powershell
cmake -S .\CppEnvironment -B .\CppEnvironment\build
cmake --build .\CppEnvironment\build
```

The CMake project defines two executable targets:

- `AnalyzisEngine` (the target name as currently spelled in `CMakeLists.txt`)
- `MatrixDetCalc`

Executables are placed in the build directory (the exact configuration subdirectory can vary by generator). For example, with a single-configuration generator on Windows:

```powershell
.\CppEnvironment\build\AnalyzisEngine.exe
.\CppEnvironment\build\MatrixDetCalc.exe
```

## Analysis executable: current behavior

`AnalyzisEngine` reads one line from standard input as an expression, parses it, differentiates with respect to `x`, and writes the result. For example:

```text
Input:  x^2 + sin(y)
Output: The partial differential is: (((2 * (x ^ (2 - 1))) * 1) + (cos(y) * 0))
```

The output is intentionally a direct representation of the derivative tree. It is not simplified, so terms such as `cos(y) * 0` remain. The executable currently fixes the differentiation variable to `x`; although the expression API accepts a variable character, the command-line program does not ask the user to select one.

The parser removes whitespace and recognizes:

| Syntax | Examples |
| --- | --- |
| Variables | `x`, `y`, `z` |
| Numeric literals | `3`, `2.5` |
| Binary arithmetic | `+`, `-`, `*`, `/`, `^` |
| Parentheses | `(x + y) * z` |
| Unary minus | `-x`, `-(x + 1)` |
| Functions | `sin(expr)`, `cos(expr)`, `exp(expr)`, `log(expr)` |

The parser is intentionally small and does not provide a general-purpose mathematical language. In particular, implicit multiplication (`2x`), named constants (`pi`, `e`), comparisons, multi-argument functions, and alternate function spellings are not supported. Use explicit `*` and lowercase function names.

The differentiator implements constants and variables, sum/difference, product and quotient rules, and chain rules for `sin`, `cos`, `exp`, and `log`. Power differentiation covers constant exponents and the current implementation's limited constant-base case. A general variable-base, variable-exponent expression (such as `x^y`) is explicitly unsupported and raises an error. For powers, “constant” is currently recognized only when the corresponding subtree is a numeric literal or a variable other than the differentiation variable; more complex expressions independent of the differentiation variable are not recognized as constant.

## Determinant executable

`MatrixDetCalc` reads a positive dimension `n`, followed by `n × n` integer entries, and prints the determinant:

```text
Input:
2
1 2
3 4

Output:
-2
```

It uses recursive cofactor expansion and stores entries and results as `long long`. This is a simple standalone utility rather than a general matrix library: large matrices can be very slow, and intermediate/results can overflow the integer type. The command-line input checks that a positive dimension and all entries are supplied.

## Planned direction

The intended destination is local-extremum analysis for implicit relations involving three variables. An important design decision is to define precisely **what quantity is being optimized**: a single equation `F(x,y,z)=0` describes a surface and does not, by itself, specify an objective function with isolated extrema.

One useful supported interpretation would be extrema of one coordinate on the level surface. For example, where `F_z != 0`, the implicit function theorem locally represents the surface as `z = z(x,y)`. Its first derivatives follow from implicit differentiation:

```text
z_x = -F_x / F_z
z_y = -F_y / F_z
```

Interior stationary points of this local height function satisfy `F = 0`, `F_x = 0`, and `F_y = 0` (with the regularity condition `F_z != 0`). The Hessian of `z(x,y)` at each candidate can then be used to classify a strict local minimum, strict local maximum, or inconclusive/degenerate point. Other interpretations—such as optimizing a separate objective `g(x,y,z)` subject to `F=0`—need a different stationary-point system, typically using Lagrange multipliers and a second-order test restricted to the constraint's tangent space.

### Suggested implementation milestones

1. **Expression correctness and evaluation**
   - Define and test the expression grammar, precedence, associativity, and malformed-input behavior.
   - Add numeric evaluation for a variable assignment and report domain errors (for example, `log(u)` when `u <= 0`).
   - Make parser errors available to callers instead of swallowing them in the `Expression` constructor.
2. **Symbolic derivatives**
   - Expand derivative coverage, including general `u^v` where the real-domain assumptions are valid.
   - Add simplification and derivative tests for each rule and combinations of rules.
3. **Reusable multivariable calculus**
   - Expose a gradient/Jacobian for `F` and a Hessian assembled from second partial derivatives.
   - Establish consistent variable ordering (`x`, `y`, `z`) and matrix conventions.
4. **Stationary-point solving**
   - Choose and document the optimization interpretation (for example, coordinate extrema on `F=0`).
   - Solve the resulting nonlinear system numerically from user-provided initial guesses or a search domain.
   - Check convergence, residuals, singular Jacobians, duplicate candidates, and domain constraints.
5. **Classification and reporting**
   - Apply a mathematically appropriate Hessian/second-order test for the chosen problem.
   - Report candidate coordinates, residuals, classification, and inconclusive cases rather than implying every candidate is classifiable.
6. **Verification**
   - Add unit tests for parsing, evaluation, derivatives, matrix operations, stationary systems, and known extrema.
   - Compare symbolic/numerical derivatives against finite-difference checks where practical.

Numerical solvers generally cannot promise to find every solution for an arbitrary nonlinear expression. Any future “for any implicit function” interface should state its supported expression domain, search strategy, and convergence limitations.

## Current limitations

- The analysis executable computes only one first partial derivative per run and always selects `x`.
- There is no expression evaluator, solver, Jacobian/Hessian API, or extremum classifier.
- Expressions are represented with `double` constants, but the determinant utility uses integer arithmetic; these are separate components.
- The parser and differentiation engine have no accompanying automated tests in the current project.
- Error handling and input validation in the analysis path are still early-stage.
