import { defineStore } from "pinia";
import { computed, ref } from "vue";
import { tokenService } from "../services/tokenService";
import { userService } from "../services/userService";
import type { LoginResponseDto } from "../Models/responseDtos";
import { useRouter } from "vue-router";

export const useUserStore = defineStore("userStore", () => {
  const token = ref<string | null>(tokenService.getToken());
  const refreshToken = ref<string | null>(tokenService.getRefreshToken());
  const userId = ref<string | null>(userService.getUserId());
  const router = useRouter();
  const isLoggedIn = computed(() => !!token.value && !!userId.value);
  let refreshTimeout = null as ReturnType<typeof setTimeout> | null;

  function login(response: LoginResponseDto) {
    token.value = response.authToken;
    userId.value = response.userId;
    refreshToken.value = response.refreshToken;

    scheduleRefresh();

    tokenService.setToken(token.value);
    tokenService.setRefreshToken(refreshToken.value);
    userService.setUserId(userId.value);
  }

  function logout() {
    token.value = null;
    userId.value = null;
    refreshToken.value = null;

    tokenService.removeToken();
    tokenService.removeRefreshToken();
    userService.removeUserId();

    router.push("/");
  }

  function initialize() {
    token.value = tokenService.getToken();
    userId.value = userService.getUserId();
    refreshToken.value = tokenService.getRefreshToken();
  }

  function getTokenExp(token: string | null) {
    if (token == null) return;
    const base64Payload = token.split(".")[1];
    const payload = atob(base64Payload!);
    const exp = JSON.parse(payload).exp;
    const expDate = new Date(exp * 1000);
    return expDate;
  }

  function isTokenExpired(): boolean {
    const exp = getTokenExp(token.value);
    if (!exp) return true;
    return exp.getTime() <= Date.now();
  }

  async function scheduleRefresh(): Promise<void> {
    const expDate = getTokenExp(token.value);
    if (!expDate) return;

    const msUntilRefresh = expDate.getTime() - Date.now() - 30_000;

    if (msUntilRefresh <= 0) {
      refreshTokenOrLogout();
      return;
    }

    if (refreshTimeout) clearTimeout(refreshTimeout);

    refreshTimeout = setTimeout(() => {
      refreshTokenOrLogout();
    }, msUntilRefresh);
  }

  async function refreshTokenOrLogout(){
    if(!isLoggedIn) return;

    try{
      if(refreshToken.value == null) {
        logout();
        return;
      }

      const data = await userService.refresh(refreshToken.value);
      login(data);
    }catch(e: any){
      logout();
      console.log(e);
    };
  }

  return {
    userId,
    token,
    refreshToken,
    isLoggedIn,
    login,
    logout,
    initialize,
    isTokenExpired,
    scheduleRefresh
  };
});
