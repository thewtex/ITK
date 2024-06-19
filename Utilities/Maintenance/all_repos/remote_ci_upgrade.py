from __future__ import annotations

import argparse
import sys
from typing import Sequence
from pathlib import Path

from all_repos import autofix_lib
from all_repos.config import Config
from all_repos.grep import repos_matching

ci_version = '5.4.0'

remote_action_config = f"""
name: Build, test, package

on: [push,pull_request]

jobs:
  cxx-build-workflow:
    uses: InsightSoftwareConsortium/ITKRemoteModuleBuildTestPackageAction/.github/workflows/build-test-cxx.yml@v{ci_version}

  python-build-workflow:
    uses: InsightSoftwareConsortium/ITKRemoteModuleBuildTestPackageAction/.github/workflows/build-test-package-python.yml@v{ci_version}
    with:
      test-notebooks: false
    secrets:
      pypi_password: ${{{{ secrets.pypi_password }}}}
"""

def find_repos(config: Config) -> set[str]:
    return repos_matching(config, ('=', '--', '.github/workflows/build-test-package.yml'))

def apply_fix() -> None:
    import yaml
    with open('.github/workflows/build-test-package.yml', 'r') as fp:
        ci_config = yaml.safe_load(fp)
        # Upgrade to the new action configured in the repository
        if not 'jobs' in ci_config or not 'cxx-build-workflow' in ci_config['jobs']:
            with open('.github/workflows/build-test-package.yml', 'w') as fp:
                fp.write(remote_action_config)
            return
        cxx_uses_old = ci_config['jobs']['cxx-build-workflow']['uses']
        cxx_uses_new = '@'.join(cxx_uses_old.split('@')[:-1] + [f'v{ci_version}'])

        python_uses_old = ci_config['jobs']['python-build-workflow']['uses']
        python_uses_new = '@'.join(python_uses_old.split('@')[:-1] + [f'v{ci_version}'])

    with open('.github/workflows/build-test-package.yml', 'r') as fp:
        ci_config = fp.read()
    with open('.github/workflows/build-test-package.yml', 'w') as fp:
        ci_config = ci_config.replace(cxx_uses_old, cxx_uses_new)
        ci_config = ci_config.replace(python_uses_old, python_uses_new)
        fp.write(ci_config)

def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    autofix_lib.add_fixer_args(parser)
    args = parser.parse_args(argv)

    autofix_lib.assert_importable(
        'yaml', install='pyyaml',
    )

    repos, config, commit, autofix_settings = autofix_lib.from_cli(
        args,
        find_repos=find_repos,
        msg=f"""ENH: Upgrade CI for ITK {ci_version}

Upgrade CI testing to use the latest ITK {ci_version} version and CI configuration.
""",
        branch_name=f'ci-itk-{ci_version}-secret-syntax',
    )

    autofix_lib.fix(
        repos,
        apply_fix=apply_fix,
        config=config,
        commit=commit,
        autofix_settings=autofix_settings,
    )
    return 0


if __name__ == '__main__':
    raise SystemExit(main())