/*
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Applications/Browser/History.h"
#include <AK/Vector.h>
#include <LibGUI/Model.h>
#include <LibGUI/Widget.h>

namespace Browser {

class HistoryModel final : public GUI::Model {
public:
    enum Column {
        Title,
        URL,
        __Count,
    };

    void set_items(AK::Vector<History::URLTitlePair> items);
    void clear_items();
    virtual int row_count(GUI::ModelIndex const&) const override;
    virtual int column_count(GUI::ModelIndex const& = GUI::ModelIndex()) const override { return Column::__Count; }
    virtual DeprecatedString column_name(int column) const override;
    virtual GUI::ModelIndex index(int row, int column = 0, GUI::ModelIndex const& = GUI::ModelIndex()) const override;
    virtual GUI::Variant data(GUI::ModelIndex const& index, GUI::ModelRole role = GUI::ModelRole::Display) const override;
    virtual TriState data_matches(GUI::ModelIndex const& index, GUI::Variant const& term) const override;

private:
    AK::Vector<History::URLTitlePair> m_entries;
};

}
