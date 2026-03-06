.PHONY: help verify-bootstrap

help:
	@echo "Targets:"
	@echo "  verify-bootstrap  - verify bootstrap structure exists"

verify-bootstrap:
	@test -f AGENTS.md
	@test -f docs/spec.md
	@test -f docs/plan.md
	@test -f docs/architecture.md
	@test -f docs/decisions.md
	@test -f docs/state_snapshot.md
	@test -f workflow/task_template.md
	@test -f workflow/report_template.md
	@test -d artifacts/reports
	@test -d tasks/active
	@test -d tasks/completed
	@echo "Bootstrap verification: PASS"
