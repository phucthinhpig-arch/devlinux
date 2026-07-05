CC := gcc
CFLAGS := -Wall -Wextra -std=c99

CPPCHECK := cppcheck
CLANG_TIDY := clang-tidy

.PHONY: build
build: main.c
	$(CC) $(CFLAGS) -o main main.c

.PHONY: cppcheck
cppcheck:
	@echo "--- Running Cppcheck ---"
	$(CPPCHECK) --enable=all --inconclusive --std=c99 main.c

.PHONY: cppcheck-report
cppcheck-report:
	@echo "--- Generating Cppcheck HTML Report ---"
	$(CPPCHECK) --enable=all --inconclusive --std=c99 --xml main.c 2> cppcheck_report.xml
	cppcheck-htmlreport --file=cppcheck_report.xml --report-dir=cppcheck_html_report --source-dir=.
	@echo "Report generated! Open cppcheck_html_report/index.html in your web browser."

.PHONY: clang-tidy
clang-tidy:
	@echo "--- Running Clang-Tidy ---"
	$(CLANG_TIDY) main.c -checks='clang-analyzer-*,cert-*,bugprone-*' --

.PHONY: clang-tidy-report
clang-tidy-report:
	@echo "--- Generating Clang-Tidy Report & Fixes ---"
	$(CLANG_TIDY) main.c -checks='clang-analyzer-*,cert-*,bugprone-*' --export-fixes=clang_tidy_fixes.yaml -- > clang_tidy_report.txt
	@echo "Report saved to clang_tidy_report.txt"
	@echo "Auto-fixes saved to clang_tidy_fixes.yaml (can be applied via IDE or clang-apply-replacements)"

.PHONY: clean
clean:
	rm -f main cppcheck_report.xml clang_tidy_report.txt clang_tidy_fixes.yaml
	rm -rf cppcheck_html_report
