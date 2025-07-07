#!/usr/bin/env sh

# Options
#
#   -V, --verbose
#     Enable verbose output for the installer
#
#   -f, -y, --force, --yes
#     Skip the confirmation prompt during installation

CREN_VERSION="0.1.0"
GITHUB_URL="https://github.com/veeso/cren/releases/download/v${CREN_VERSION}"
LINUX_X86_PKG_NAME="cren-v${CREN_VERSION}-x86_64-unknown-linux-gnu.tar.gz"
LINUX_AARCH64_PKG_NAME="cren-v${CREN_VERSION}-aarch64-unknown-linux-gnu.tar.gz"
MACOS_X86_PKG_NAME="cren-v${CREN_VERSION}-x86_64-apple-darwin.tar.gz"
MACOS_AARCH64_PKG_NAME="cren-v${CREN_VERSION}-aarch64-apple-darwin.tar.gz"

PATH="$PATH:/usr/sbin"

set -eu
printf "\n"

BOLD="$(tput bold 2>/dev/null || printf '')"
GREY="$(tput setaf 0 2>/dev/null || printf '')"
UNDERLINE="$(tput smul 2>/dev/null || printf '')"
RED="$(tput setaf 1 2>/dev/null || printf '')"
GREEN="$(tput setaf 2 2>/dev/null || printf '')"
YELLOW="$(tput setaf 3 2>/dev/null || printf '')"
BLUE="$(tput setaf 4 2>/dev/null || printf '')"
MAGENTA="$(tput setaf 5 2>/dev/null || printf '')"
NO_COLOR="$(tput sgr0 2>/dev/null || printf '')"

# Functions

info() {
    printf '%s\n' "${BOLD}${GREY}>${NO_COLOR} $*"
}

warn() {
    printf '%s\n' "${YELLOW}! $*${NO_COLOR}"
}

error() {
    printf '%s\n' "${RED}x $*${NO_COLOR}" >&2
}

completed() {
    printf '%s\n' "${GREEN}✓${NO_COLOR} $*"
}

has() {
    command -v "$1" 1>/dev/null 2>&1
}

set_cren_version() {
    CREN_VERSION="$1"
    info "Setting cren version to ${CREN_VERSION}"
    GITHUB_URL="https://github.com/veeso/cren/releases/download/v${CREN_VERSION}"
    LINUX_X86_PKG_NAME="cren-v${CREN_VERSION}-x86_64-unknown-linux-gnu.tar.gz"
    LINUX_AARCH64_PKG_NAME="cren-v${CREN_VERSION}-aarch64-unknown-linux-gnu.tar.gz"
    MACOS_X86_PKG_NAME="cren-v${CREN_VERSION}-x86_64-apple-darwin.tar.gz"
    MACOS_AARCH64_PKG_NAME="cren-v${CREN_VERSION}-aarch64-apple-darwin.tar.gz"
}

get_tmpfile() {
    local suffix
    suffix="$1"
    if has mktemp; then
        printf "%s.%s" "$(mktemp)" "${suffix}"
    else
        # No really good options here--let's pick a default + hope
        printf "/tmp/cren.%s" "${suffix}"
    fi
}

get_tmpdir() {
    if has mktemp; then
        printf "%s" "$(mktemp -d)"
    else
        # No really good options here--let's pick a default + hope
        printf "/tmp/cren_${date +%Y%m%d_%H%M%S}"
    fi
}

download() {
    output="$1"
    url="$2"
    
    if has curl; then
        cmd="curl --fail --silent --location --output $output $url"
    elif has wget; then
        cmd="wget --quiet --output-document=$output $url"
    elif has fetch; then
        cmd="fetch --quiet --output=$output $url"
    else
        error "No HTTP download program (curl, wget, fetch) found, exiting…"
        return 1
    fi
    $cmd && return 0 || rc=$?
    
    error "Command failed (exit code $rc): ${BLUE}${cmd}${NO_COLOR}"
    warn "If you believe this is a bug, please report immediately an issue to <https://github.com/veeso/cren/issues/new>"
    return $rc
}

test_writeable() {
  local path
  path="${1:-}/test.txt"
  if touch "${path}" 2>/dev/null; then
    rm "${path}"
    return 0
  else
    return 1
  fi
}

elevate_priv() {
    if has sudo; then
      if ! sudo -v; then
        error "Superuser not granted, aborting installation"
        exit 1
      fi
      sudo="sudo"
    elif has doas; then
      sudo="doas"
    else
      error 'Could not find the commands "sudo" or "doas", needed to install cren on your system.'
      info "If you are on Windows, please run your shell as an administrator, then"
      info "rerun this script. Otherwise, please run this script as root, or install"
      info "sudo or doas."
      exit 1
    fi
}

elevate_priv_ex() {
    check_dir="$1"
    if test_writeable "$check_dir"; then
        sudo=""
    else
        elevate_priv
    fi
    echo $sudo
}

# Currently supporting:
#   - macos
#   - linux
#   - freebsd
detect_platform() {
    local platform
    platform="$(uname -s | tr '[:upper:]' '[:lower:]')"
    
    case "${platform}" in
        linux) platform="linux" ;;
        darwin) platform="macos" ;;
        freebsd) platform="freebsd" ;;
    esac
    
    printf '%s' "${platform}"
}

# Currently supporting:
#   - x86_64
detect_arch() {
    local arch
    arch="$(uname -m | tr '[:upper:]' '[:lower:]')"
    
    case "${arch}" in
        amd64) arch="x86_64" ;;
        armv*) arch="arm" ;;
        arm64) arch="aarch64" ;;
    esac
    
    # `uname -m` in some cases mis-reports 32-bit OS as 64-bit, so double check
    if [ "${arch}" = "x86_64" ] && [ "$(getconf LONG_BIT)" -eq 32 ]; then
        arch="i686"
    elif [ "${arch}" = "aarch64" ] && [ "$(getconf LONG_BIT)" -eq 32 ]; then
        arch="arm"
    fi
    
    printf '%s' "${arch}"
}

confirm() {
    if [ -z "${FORCE-}" ]; then
        printf "%s " "${MAGENTA}?${NO_COLOR} $* ${BOLD}[y/N]${NO_COLOR}"
        set +e
        read -r yn </dev/tty
        rc=$?
        set -e
        if [ $rc -ne 0 ]; then
            error "Error reading from prompt (please re-run with the '--yes' option)"
            exit 1
        fi
        if [ "$yn" != "y" ] && [ "$yn" != "yes" ]; then
            error 'Aborting (please answer "yes" to continue)'
            exit 1
        fi
    fi
}

# Installers

install_from_source() {
    CURRDIR="$(pwd)"

    if ! has git; then
        error "git is not installed, please install it to continue"
        exit 1
    fi

    if ! has cmake; then
        error "cmake is not installed, please install it to continue"
        exit 1
    fi
    
    # clone the repository
    info "Cloning the cren repository"
    local tmpdir
    tmpdir="$(get_tmpdir)"

    if ! git clone https://github.com/veeso/cren.git $tmpdir; then
        error "Failed to clone the cren repository"
        exit 1
    fi

    # change directory to the cloned repository
    cd $tmpdir || {
        error "Failed to change directory to $tmpdir"
        exit 1
    }

    # checkout the specific version
    info "Checking out version ${CREN_VERSION}"
    if ! git checkout "v${CREN_VERSION}"; then
        error "Failed to checkout version ${CREN_VERSION}"
        exit 1
    fi

    # create build directory
    info "Creating build directory"
    mkdir -p build || {
        error "Failed to create or change to build directory"
        exit 1
    }

    # run cmake
    info "Running cmake"
    if ! cmake -B build -S . -DCMAKE_BUILD_TYPE=Release; then
        error "Failed to run cmake"
        exit 1
    fi

    # build the project
    info "Building cren"
    if ! cmake --build build --config Release; then
        error "Failed to build cren"
        exit 1
    fi

    # become root if needed
    sudo="$(elevate_priv_ex $BIN_DIR)"
    # install the binary
    info "Installing cren to ${BIN_DIR}"
    if ! $sudo cp build/cren "${BIN_DIR}"; then
        error "Failed to install cren to ${BIN_DIR}"
        exit 1
    fi

    # cleanup
    cd $CURRDIR
    info "Cleaning up temporary files"
    if [ -d $tmpdir ]; then
        rm -rf $tmpdir
    fi

    info "Installed ${BOLD}${GREEN}cren ${CREN_VERSION}${NO_COLOR} successfully at ${BOLD}${GREEN}$(which cren)${CREN_VERSION}${NO_COLOR}"
}

install_pkg() {
    local pkg_name="$1"
    local pkg_url="${GITHUB_URL}/${pkg_name}"
    local tmpfile
    tmpfile="$(get_tmpfile "tar.gz")"

    # download
    info "Downloading ${pkg_name} from ${GITHUB_URL}"
    if ! download "${tmpfile}" "${pkg_url}"; then
        error "Failed to download ${pkg_name} from ${GITHUB_URL}"
        exit 1
    fi

    # become root if needed
    sudo="$(elevate_priv_ex "${BIN_DIR}")"

    # extract
    info "Extracting ${pkg_name} to ${BIN_DIR}"
    if ! $sudo tar -xzf "${tmpfile}" -C "${BIN_DIR}" --strip-components=1; then
        error "Failed to extract ${pkg_name} to ${BIN_DIR}"
        exit 1
    fi
    
    # cleanup
    info "Cleaning up temporary files"
    if [ -f "${tmpfile}" ]; then
        rm -f "${tmpfile}"
    fi

    info "Installed ${BOLD}${GREEN}cren ${CREN_VERSION}${NO_COLOR} successfully at ${BOLD}${GREEN}$(which cren)${CREN_VERSION}${NO_COLOR}"
}


# defaults
if [ -z "${PLATFORM-}" ]; then
    PLATFORM="$(detect_platform)"
fi

if [ -z "${BIN_DIR-}" ]; then
    BIN_DIR=/usr/local/bin
fi

if [ -z "${ARCH-}" ]; then
    ARCH="$(detect_arch)"
fi

if [ -z "${BUILD_FROM_SOURCE-}" ]; then
    BUILD_FROM_SOURCE=0
fi

# parse argv variables
while [ "$#" -gt 0 ]; do
    echo $1
    case "$1" in
        -b=* | --bin-dir=*)
            BIN_DIR="${1#*=}"
            if [ ! -d "${BIN_DIR}" ]; then
                error "The specified bin directory does not exist: ${BIN_DIR}"
                exit 1
            fi
            shift 1
        ;;

        -s | --source)
            BUILD_FROM_SOURCE=1
            shift 1
        ;;

        -f | -y | --force | --yes)
            FORCE=1
            shift 1
        ;;

        -f=* | -y=* | --force=* | --yes=*)
            FORCE="${1#*=}"
            shift 1
        ;;

        -v=* | --version=*)
            set_cren_version "${1#*=}"
            shift 1
        ;;

        *)
            error "Unknown option: $1"
            exit 1
        ;;
    esac
done

printf "  %s\n" "${UNDERLINE}cren configuration${NO_COLOR}"
info "${BOLD}Platform${NO_COLOR}:      ${GREEN}${PLATFORM}${NO_COLOR}"
info "${BOLD}Arch${NO_COLOR}:          ${GREEN}${ARCH}${NO_COLOR}"
info "${BOLD}Bin directory${NO_COLOR}: ${GREEN}${BIN_DIR}${NO_COLOR}"
if [ "${BUILD_FROM_SOURCE}" -eq 1 ]; then
    info "${BOLD}Build from source${NO_COLOR}: ${GREEN}yes${NO_COLOR}"
else
    info "${BOLD}Build from source${NO_COLOR}: ${GREEN}no${NO_COLOR}"
fi
info "${BOLD}Version${NO_COLOR}:       ${GREEN}${CREN_VERSION}${NO_COLOR}"
printf "  %s\n" "${UNDERLINE}End of configuration${NO_COLOR}"

printf "\n"

confirm "Install ${GREEN}cren ${CREN_VERSION}${NO_COLOR}?"

TUPLE="${PLATFORM}-${ARCH}"
info "Detected platform/arch tuple: ${TUPLE}"
# verify that the tuple is supported
case "${TUPLE}" in
    "linux-x86_64")
        PKG_NAME="${LINUX_X86_PKG_NAME}"
    ;;
    "linux-aarch64")
        PKG_NAME="${LINUX_AARCH64_PKG_NAME}"
    ;;
    "macos-x86_64")
        PKG_NAME="${MACOS_X86_PKG_NAME}"
    ;;
    "macos-aarch64")
        PKG_NAME="${MACOS_AARCH64_PKG_NAME}"
    ;;
    *)
        PKG_NAME=""
        warn "Unsupported platform/arch tuple: ${TUPLE}; Building from source"
    ;;
esac

# force source
if [ "${BUILD_FROM_SOURCE}" -eq 1 ]; then
    info "Building from source"
    PKG_NAME=""
fi

# Installation from package or source
if [ -n "${PKG_NAME}" ]; then
    install_pkg "${PKG_NAME}"
else
    install_from_source
fi

completed "Congratulations! cren has successfully been installed on your system!"
info "If you're a new user, you can discover more on <https://cren.cc>"
info "Remember that if you encounter any issue, you can report them on Github <https://github.com/veeso/cren/issues/new>"
info "Feel free to open an issue also if you have an idea which could improve the project"
info "If you want to support the project, please, consider a little donation <https://ko-fi.com/veeso>"
info "I hope you'll enjoy using cren :D"

exit 0
