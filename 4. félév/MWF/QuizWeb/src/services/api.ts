import axios from "axios";
import { tokenService } from "./tokenService";
import { useUserStore } from "../stores/userStore";

export const api = axios.create({
  baseURL: "/api/",
  headers: {
    Accept: "application/json",
    "Content-Type": "application/json",
  },
});

api.interceptors.request.use((config) => {
  const token = tokenService.getToken();
  if (token) {
    config.headers = config.headers || {};
    config.headers.Authorization = `Bearer ${token}`;
  }
  return config;
});

api.interceptors.response.use(
  (response) => response,
  (error) => {
    const message =
      error.response?.data?.detail ||
      error.response?.data?.title ||
      "Something went wrong";

    const status = error.response?.status;

    if (status === 401) {
      const userStore = useUserStore();
      userStore.logout();
    }

    return Promise.reject({ message, status });
  },
);
