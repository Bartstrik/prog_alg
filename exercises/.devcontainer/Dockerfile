FROM intel/oneapi-hpckit:2025.2.2-0-devel-ubuntu24.04

RUN wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB | gpg --dearmor | tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
RUN echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | tee /etc/apt/sources.list.d/oneAPI.list
RUN apt-key del 7fa2af80
RUN wget https://developer.download.nvidia.com/compute/cuda/repos/wsl-ubuntu/x86_64/cuda-keyring_1.1-1_all.deb
RUN dpkg -i cuda-keyring_1.1-1_all.deb

# CUDA Toolkit 12.9 is highest supported version with oneapi-for-nvidia-gpus-2025.2.0-linux.sh
RUN apt-get update && \
    apt-get install -y git wget gnupg2 cmake pkg-config build-essential libfreeimage-dev libfreeimageplus-dev cuda-toolkit-12-9 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

COPY oneapi-for-nvidia-gpus-2025.2.0-linux.sh /tmp/
RUN chmod +x /tmp/oneapi-for-nvidia-gpus-2025.2.0-linux.sh && \
    /tmp/oneapi-for-nvidia-gpus-2025.2.0-linux.sh --install-dir /opt/intel/oneapi -y
RUN rm /tmp/oneapi-for-nvidia-gpus-2025.2.0-linux.sh

RUN echo 'source /opt/intel/oneapi/setvars.sh' >> /etc/bash.bashrc

RUN echo '#!/bin/bash' > /usr/local/bin/with-oneapi && \
    echo 'source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1' >> /usr/local/bin/with-oneapi && \
    echo 'exec "$@"' >> /usr/local/bin/with-oneapi && \
    chmod +x /usr/local/bin/with-oneapi

ENTRYPOINT ["/usr/local/bin/with-oneapi"]