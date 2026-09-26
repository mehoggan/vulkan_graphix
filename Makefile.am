SUBDIRS = lib bin tests vulkan_earth/src
ACLOCAL_AMFLAGS = -I m4
CLEANFILES = *.o
CLEANDIRS = deps/ .lib/

# Code coverage: configure with --enable-coverage first (see README.md).
# Filtered to this project's own lib/ implementation and include/vulkan_graphix/
# headers - system headers, glm, gtest, and the vendored STBImage.h/
# ListOfFunctions.inl are excluded since they aren't ours to cover.
if COVERAGE_ENABLED
GCOVR_FILTERS = --filter 'lib/.*' \
	--filter 'include/vulkan_graphix/.*' \
	--exclude 'include/vulkan_graphix/STBImage\.h' \
	--exclude 'include/vulkan_graphix/ListOfFunctions\.inl'

.PHONY: coverage coverage-missing coverage-clean

coverage-clean:
	find $(top_builddir) -name '*.gcda' -delete
	rm -rf coverage-html coverage.info coverage-baseline.info coverage-run.info

# Rebuilds and re-runs the test suite, then writes an HTML report (one
# page per file, uncovered lines highlighted) to coverage-html/index.html.
coverage: coverage-clean
	$(MAKE) check
	@if [ -n "$(GCOVR)" ]; then \
		mkdir -p coverage-html; \
		$(GCOVR) --root $(top_builddir) $(GCOVR_FILTERS) \
			--html --html-details -o coverage-html/index.html \
			--print-summary; \
		echo "HTML coverage report: coverage-html/index.html"; \
	elif [ -n "$(LCOV)" ] && [ -n "$(GENHTML)" ]; then \
		$(LCOV) --capture --initial --directory $(top_builddir) \
			--ignore-errors inconsistent,negative \
			--output-file coverage-baseline.info; \
		$(LCOV) --capture --directory $(top_builddir) \
			--ignore-errors inconsistent \
			--output-file coverage-run.info; \
		$(LCOV) --add-tracefile coverage-baseline.info \
			--add-tracefile coverage-run.info \
			--ignore-errors inconsistent,negative,count \
			--output-file coverage.info; \
		$(LCOV) --remove coverage.info '/usr/*' \
			'*/tests/*' '*STBImage.h' '*ListOfFunctions.inl' \
			--ignore-errors inconsistent,unused \
			--output-file coverage.info; \
		rm -f coverage-baseline.info coverage-run.info; \
		$(GENHTML) coverage.info --ignore-errors inconsistent,category \
			--output-directory coverage-html; \
		echo "HTML coverage report: coverage-html/index.html"; \
	else \
		echo "Install gcovr (pip install --user gcovr) or lcov" \
			"(apt install lcov) to turn the .gcda/.gcno files into a report."; \
	fi

# Prints exactly which lines aren't covered, per file - no HTML needed.
# gcovr-only: its --txt report has a "Missing" column of line numbers/
# ranges that lcov has no equivalent for (lcov's own text output, `lcov
# --list`, only gives per-file percentages - see uncovered lines via the
# highlighted source in `make coverage`'s HTML report instead).
# Does not rebuild/re-run tests; run `make coverage` (or `make check`)
# first if the .gcda files might be stale.
coverage-missing:
	@if [ -n "$(GCOVR)" ]; then \
		$(GCOVR) --root $(top_builddir) $(GCOVR_FILTERS) --txt; \
	else \
		echo "This needs gcovr specifically (lcov has no per-line" \
			"missing-lines report): pip install --user gcovr." \
			"Or see uncovered lines highlighted in the HTML from" \
			"'make coverage'."; \
	fi

clean-local:
	find $(top_builddir) \( -name '*.gcno' -o -name '*.gcda' \) -delete
	rm -rf coverage-html coverage.info coverage-baseline.info coverage-run.info
endif
