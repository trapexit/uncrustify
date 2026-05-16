/**
 * @file annotations.cpp
 *
 * @author  Ben Gardner
 * @author  Guy Maurel
 * @license GPL v2+
 */

#include "newlines/annotations.h"

#include "chunk.h"
#include "language_tools.h"
#include "log_rules.h"
#include "newlines/iarf.h"
#include "options.h"


using namespace uncrustify;


constexpr static auto LCURRENT = LNEWLINE;


static bool is_c_or_cpp_attribute(Chunk *pc)
{
   return(  pc->Is(E_Token::CT_ATTRIBUTE)
         && (  language_is_set(lang_flag_e::LANG_C)
            || language_is_set(lang_flag_e::LANG_CPP)));
}


static bool is_annotation_or_attribute(Chunk *pc)
{
   return(  pc->Is(E_Token::CT_ANNOTATION)
         || is_c_or_cpp_attribute(pc));
}


static Chunk *get_annotation_end(Chunk *pc)
{
   Chunk *next = pc->GetNextNnl();

   if (next->IsParenOpen())
   {
      return(next->GetClosingParen());
   }
   return(pc);
}


static bool can_precede_attribute_statement(Chunk *pc)
{
   return(  pc->Is(E_Token::CT_CASE_COLON)
         || pc->Is(E_Token::CT_SEMICOLON)
         || pc->Is(E_Token::CT_BRACE_OPEN)
         || pc->Is(E_Token::CT_BRACE_CLOSE)
         || pc->Is(E_Token::CT_VBRACE_OPEN)
         || pc->Is(E_Token::CT_VBRACE_CLOSE));
}


void annotations_newlines()
{
   LOG_FUNC_ENTRY();

   Chunk *next;
   Chunk *ae;   // last token of the annotation
   Chunk *pc = Chunk::GetHead();

   while (pc->IsNotNullChunk())
   {
      if (!is_annotation_or_attribute(pc))
      {
         pc = pc->GetNext();
         continue;
      }
      // find the end of this annotation
      ae = get_annotation_end(pc);

      if (ae->IsNullChunk())
      {
         break;
      }
      LOG_FMT(LANNOT, "%s(%d): orig line is %zu, orig col is %zu, annotation is '%s',  end @ orig line %zu, orig col %zu, is '%s'\n",
              __func__, __LINE__, pc->GetOrigLine(), pc->GetOrigCol(), pc->GetLogText(),
              ae->GetOrigLine(), ae->GetOrigCol(), ae->GetLogText());

      Chunk const *prev;
      prev = ae->GetPrev();             // Issue #1845
      LOG_FMT(LANNOT, "%s(%d): prev orig line is %zu, orig col is %zu, text is '%s'\n",
              __func__, __LINE__, prev->GetOrigLine(), prev->GetOrigCol(), prev->GetLogText());
      next = ae->GetNextNnl();

      if (is_annotation_or_attribute(next))
      {
         LOG_FMT(LANNOT, "%s(%d):  -- nl_between_annotation\n",
                 __func__, __LINE__);
         newline_iarf(ae, uncrustify::options::nl_between_annotation());
         log_rule_B("nl_between_annotation");
      }
      else if (  is_c_or_cpp_attribute(pc)
              && next->IsNotNullChunk())
      {
         if (next->Is(E_Token::CT_SEMICOLON))
         {
            Chunk *prev_non_nl = pc->GetPrevNnl();

            if (  prev_non_nl->IsNotNullChunk()
               && can_precede_attribute_statement(prev_non_nl))
            {
               LOG_FMT(LANNOT, "%s(%d):  -- nl_after_annotation\n",
                       __func__, __LINE__);
               newline_iarf_pair(prev_non_nl, pc, options::nl_after_annotation());
               log_rule_B("nl_after_annotation");
            }
            LOG_FMT(LANNOT, "%s(%d):  -- nl_after_annotation\n",
                    __func__, __LINE__);
            newline_iarf(next, options::nl_after_annotation());
            log_rule_B("nl_after_annotation");
         }
         else
         {
            LOG_FMT(LANNOT, "%s(%d):  -- nl_after_annotation\n",
                    __func__, __LINE__);
            newline_iarf(ae, options::nl_after_annotation());
            log_rule_B("nl_after_annotation");
         }
      }
      pc = ae->GetNext();
   }
} // annotations_newlines
