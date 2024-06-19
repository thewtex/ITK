import collections
from typing import Dict
from pathlib import Path
import glob

Settings = collections.namedtuple('Settings', ('itk_source_dir'))
Settings.__new__.__defaults__ = ()

def list_repos(settings: Settings) -> Dict[str, str]:
    # get the path to the current script
    remotes_dir = Path(__file__).parent.absolute() / '..' / '..' / '..' / '..' / 'Modules' / 'Remote'
    result = {}
    print(remotes_dir)
    remotes = glob.glob(str(remotes_dir / '*.remote.cmake'))
    print(remotes)
    for remote in remotes:
        with open(remote, 'r') as f:
            lines = f.readlines()
            for line in lines:
                if 'GIT_REPOSITORY' in line:
                    repo = line.split('GIT_REPOSITORY ')[-1].strip()
                    repo = repo.replace('https://github.com/', 'git@github.com:')
                    if repo.endswith('.git'):
                        repo = repo[:-4]
                    name = Path(remote).stem[:-7]
                    result[name] = repo

    # print(result)
    # return { "ITKMontage": "git@github.com:InsightSoftwareConsortium/ITKMontage" }
    return result
