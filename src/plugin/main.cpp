#include <clang/Frontend/FrontendAction.h>
#include <clang/Lex/Pragma.h>
#include <llvm/Support/Error.h>

#define ignore (void)

using namespace clang;

struct Embed : public PragmaHandler {
    std::array<char*, 0xFF + 1> numbers;
    std::array<int, 0xFF + 1> number_lengths;

    Embed()
        : PragmaHandler { "embed" }
    {
        for (size_t i = 0; i < numbers.size(); i++) {
            auto len = asprintf(&numbers[i], "%zu", i);
            number_lengths[i] = len;
        }
    }

    ~Embed()
    {
        for (auto* e : numbers)
            free(e);
    }

    void HandlePragma(Preprocessor& pp, PragmaIntroducer, Token&)
    {
        auto& file_manager = pp.getFileManager();
        auto& source_manager = pp.getSourceManager();
        auto macro_name_tok = Token();
        pp.Lex(macro_name_tok);
        if (macro_name_tok.isNot(tok::TokenKind::identifier)) {
            llvm::errs() << "expected identifier\n";
            return;
        }
        auto* macro_ident = macro_name_tok.getIdentifierInfo();
        auto file_name_tok = Token();
        std::string file_name;
        pp.LexStringLiteral(file_name_tok, file_name, nullptr, true);

        auto* info
            = pp.AllocateMacroInfo(file_name_tok.getLocation());

        auto file_or_error = file_manager.getBufferForFile(
            file_name, false, false);
        if (!file_or_error) {
            auto message = file_or_error.getError().message();
            auto file_line_and_column
                = file_name_tok.getLocation().printToString(
                    source_manager);
            llvm::errs()
                << file_line_and_column << ": could not embed '"
                << file_name << "': " << message;
            exit(1);
        }
        auto* file = file_or_error->release();
        auto const* data = file->getBufferStart();
        auto size = file->getBufferSize();

        auto comma = Token();
        comma.setKind(tok::TokenKind::comma);
        comma.setLength(1);
        comma.setLocation(file_name_tok.getLocation());

        auto number_tok = Token();
        number_tok.setKind(tok::TokenKind::numeric_constant);
        number_tok.setLocation(file_name_tok.getLocation());
        for (size_t i = 0; i < size; i++) {
            auto number = data[i];
            number_tok.setLiteralData(numbers[number]);
            number_tok.setLength(number_lengths[number]);
            info->AddTokenToBody(number_tok);
            info->AddTokenToBody(comma);
        }

        pp.appendDefMacroDirective(macro_ident, info);
    }
};

struct Pragmas : PragmaNamespace {
  Embed embed {};

  Pragmas()
      : PragmaNamespace("JR")
  {
      AddPragma(&embed);
  }

};

static auto const X = clang::PragmaHandlerRegistry::Add<Pragmas> {
    "embed",
    "embed file",
};
