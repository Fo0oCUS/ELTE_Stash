import type { UserResponseDto } from "../Models/responseDtos";
import { api } from "./api";
import { tokenService } from "./tokenService";
import type { LoginRequestDto, RegisterRequestDto } from "../Models/requestDtos";
import type { LoginResponseDto } from "../Models/responseDtos";

const USER_ID_KEY = "userId";


export const userService = {
  async getUser(): Promise<UserResponseDto> {
    const response = await api.get("/users/" + this.getUserId());
    return response.data;
  },

  async login(data: LoginRequestDto): Promise<LoginResponseDto> {
    const response = await api.post("/users/login", data);
    return response.data;
  },

  async register(data: RegisterRequestDto){
    const response = await api.post("/users", data);
    return response.data;
  },

  async refresh(token: string): Promise<LoginResponseDto>{
    const response = await api.post("/users/refresh", token);
    return response.data;
  },

  async logout() {
    const response = await api.post("/users/logout");
    tokenService.removeToken();
    this.removeUserId();
    return response.data;
  },

  isLoggedIn(): boolean {
    const uid = this.getUserId();
    return tokenService.getToken() != null && uid != null && uid != "";
  },

  setUserId(id: string) {
    localStorage.setItem(USER_ID_KEY, id);
  },

  getUserId(): string | null {
    return localStorage.getItem(USER_ID_KEY);
  },

  removeUserId() {
    localStorage.removeItem(USER_ID_KEY);
  },
};
