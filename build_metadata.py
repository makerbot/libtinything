import sys

try:
    import artifactory_utils
except:
    pass
else:
    dependencies = [
        artifactory_utils.ArtifactSelector(
            project="Toolchain-Release",
            revision="develop",
            version="^4.6",
            debug=False,
            stable_required=True),
        artifactory_utils.ArtifactSelector(
            project="json-cpp",
            revision="ReleaseMorepork0.59",
            version="3.*",
            debug=False,
            stable_required=True),
        artifactory_utils.ArtifactSelector(
            project="MBCoreUtils",
            revision="ReleaseMorepork0.59",
            version="^4.0",
            debug=False,
            stable_required=True)
    ]
    # Ideally we would be using the platform to select the appropriate
    # artifact for the above dependencies.  But instead we can just pull
    # down both artifacts, which works fine as long as the cross compiled
    # artifacts actually exist.
    if sys.platform.startswith('linux') and sys.maxsize > 2**32:
        dependencies.extend([
            artifactory_utils.ArtifactSelector(
                project="morepork-mbcoreutils",
                revision="ReleaseMorepork0.59",
                version="^4.0",
                debug=False,
                stable_required=True),
            artifactory_utils.ArtifactSelector(
                project="morepork-json-cpp",
                revision="ReleaseMorepork0.59",
                version="3.*",
                debug=False,
                stable_required=True),
        ])
