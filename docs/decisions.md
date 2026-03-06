# Super_Z80_v2 Decision Log

## ADR Template
- ID:
- Date:
- Status: Proposed | Accepted | Superseded
- Context:
- Decision:
- Rationale:
- Supersedes:
- Superseded By:

## ADR-0001
- ID: ADR-0001
- Date: 2026-03-06
- Status: Accepted
- Context: The project needs a consistent coding agent runtime.
- Decision: Use OpenAI Codex CLI as the coding agent environment.
- Rationale: It provides a repeatable execution surface aligned with repository-driven workflows.
- Supersedes: None
- Superseded By: None

## ADR-0002
- ID: ADR-0002
- Date: 2026-03-06
- Status: Accepted
- Context: Durable memory is required for stateless sessions.
- Decision: Use AWOS-style repository truth and task packets.
- Rationale: It keeps project state recoverable from repository files instead of chat context.
- Supersedes: None
- Superseded By: None

## ADR-0003
- ID: ADR-0003
- Date: 2026-03-06
- Status: Accepted
- Context: Prior efforts risked excessive design before working code.
- Decision: Prefer implementation-first development.
- Rationale: Small, working increments reduce drift and improve feedback quality.
- Supersedes: None
- Superseded By: None

## ADR-0004
- ID: ADR-0004
- Date: 2026-03-06
- Status: Accepted
- Context: Agent work must be objectively verifiable.
- Decision: Require deterministic verification for every implementation task.
- Rationale: Observable pass/fail output prevents subjective completion claims.
- Supersedes: None
- Superseded By: None

## ADR-0005
- ID: ADR-0005
- Date: 2026-03-06
- Status: Accepted
- Context: Multi-agent workflows add coordination overhead for this project stage.
- Decision: Use a single human planner, a single coding agent, and sequential development.
- Rationale: This minimizes coordination complexity and keeps accountability explicit.
- Supersedes: None
- Superseded By: None
