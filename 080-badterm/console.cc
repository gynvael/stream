#include <poll.h>
#include <algorithm>
#include "console.h"

pid_t Console::GetPid() const {
  return pid_;
}

void Console::ResetPid() {
  pid_ = -1;
}

void Console::HandleSurfaceChange(SDL_Surface *surface) {
  surface_ = surface;
}

void Console::ResizeTextBuffer(uint32_t w, uint32_t h) {
  unsigned int columns = std::max(w / char_width_, 1U);
  lines_ = std::max(h / char_height_, 1U);

  buffer_.Resize(columns, scrollback_lines_);

  // TODO: send signal to child
}

bool Console::SpawnChild() {
  master_ = getpt();
  if (master_ == -1) {
    perror("getpt");
    return false;
  }

  char slave_path[256]{};
  if (ptsname_r(master_, slave_path, sizeof(slave_path)) != 0) {
    perror("ptsname_r");
    close(master_);
    return false;
  }

  if (grantpt(master_) != 0) {
    perror("grantpt");
    close(master_);
    return false;
  }

  if (unlockpt(master_) != 0) {
    perror("unlockpt");
    close(master_);
    return false;
  }

  int slave = open(slave_path, O_RDWR);
  if (slave < 0) {
    perror("open");
    close(master_);
    return false;
  }

  pid_ = fork();
  if (pid_ == -1) {
    perror("fork");
    close(master_);
    close(slave);
    return false;
  }

  if (pid_ == 0) {
    // Child process.
    close(0);  // stdin
    close(1);  // stdout
    close(2);  // stderr
    close(master_);
    master_ = -1;

    dup2(slave, 0);
    dup2(slave, 1);
    dup2(slave, 2);

    close(slave);

    if (execl("/bin/bash", "/bin/bash", nullptr) == -1) {
      abort();
    }
  }

  // Parent process.
  close(slave);
  read_th_ = std::make_unique<std::thread>(&Console::ReaderWorker, this);

  return true;
}

void Console::ProcessOutput(uint8_t *data, size_t size) {
  // TODO: handle console control codes

  // TODO: remove me this is for debug only
  for (size_t i = 0; i < size; i++) {
    buffer_.InsertCharacter(data[i]);
  }

  UpdateSurface();
}

void Console::ReaderWorker() {
  uint8_t buf[4096];

  pollfd fds[] = {
    { master_, POLLIN | POLLPRI | POLLRDHUP | POLLERR, 0 /* ignored */ }
  };

  while (!end_threads_.load()) {

    int ret = poll(fds, 1, 50 /* ms */);
    if (ret == -1) {
      perror("poll");
      break;
    }

    if (ret == 0) {
      // Timeout.
      continue;
    }

    const int revents = fds[0].revents;

    if ((revents & POLLIN)) {
      ssize_t buf_read = read(master_, buf, sizeof(buf));
      if (buf_read == -1) {
        perror("ReaderWorker");
      }

      //fwrite(buf, 1, buf_read, stdout);
      ProcessOutput(buf, (size_t)buf_read);

    } else {
      fprintf(stderr, "poll revents == %.x\n", revents);
    }
  }
}

void Console::CloseMaster() {
  end_threads_.store(true);
  read_th_->join();
  close(master_);
  master_ = -1;
}

