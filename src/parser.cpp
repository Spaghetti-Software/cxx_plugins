/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    parser.cpp
 * \author  Andrey Ponomarev
 * \date    03 Jul 2020
 * \brief
 * $BRIEF$
 */

#include "cxx_plugins/parser.hpp"

namespace CxxPlugins::JSON {

void impl::parsingLippincott(std::string_view type_description) {
  try {
    throw;
  } catch (TypeMismatch const &type_mismatch) {
    throw TypeMismatch(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, type_mismatch.what()));
  } catch (ArraySizeMismatch const &array_size_mismatch) {
    throw ArraySizeMismatch(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, array_size_mismatch.what()));
  } catch (ObjectSizeMismatch const &object_size_mismatch) {
    throw ObjectSizeMismatch(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, object_size_mismatch.what()));
  } catch (ObjectMemberMissing const &object_member_missing) {
    throw ObjectMemberMissing(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, object_member_missing.what()));
  } catch (ParsingError const &parsing_error) {
    throw ParsingError(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, parsing_error.what()));
  } catch (std::runtime_error const &re) {
    throw std::runtime_error(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, re.what()));
  } catch (...) {
    throw;
  }
}
} // namespace CxxPlugins
