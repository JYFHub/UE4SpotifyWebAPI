// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Networking.h"
#include "GameFramework/SaveGame.h"
#include "SpotifyController.generated.h"



UENUM(BlueprintType)
enum class ESpotifyApiScopes : uint8
{
  UserLibraryRead,
  UserLibraryModify,
  PlaylistReadPrivate,
  PlaylistModifyPublic,
  PlaylistModifyPrivate,
  PlaylistReadCollaborative,
  UserReadRecentlyPlayed,
  UserTopRead,
  UserReadPrivate,
  UserReadEmail,
  UserReadBirthdate,
  Streaming,
  UserModifyPlaybackState,
  UserReadPlaybackState,
  UserReadCurrentlyPlaying,
  UserFollowModify,
  UserFollowRead
};


UCLASS()
class SPOTIFYTEST_API USpotifyTokenSave : public USaveGame
{
  GENERATED_BODY()

public:

  UPROPERTY(VisibleAnywhere, Category = "Auth")
  FString RefreshToken;


};

/**
 * 
 */
UCLASS()
class SPOTIFYTEST_API ASpotifyController : public APlayerController
{
	GENERATED_BODY()
	
public:

  ASpotifyController();

protected:

  // The ClientId Key from your Spotify App
  UPROPERTY(EditDefaultsOnly, Category = "Spotify")
  FString ClientId;

  // The Client Secret from your Spotify App
  UPROPERTY(EditDefaultsOnly, Category = "Spotify")
  FString ClientSecret;

  // The URI the user is being redirected to after auth (whether successful or not) default: http://127.0.0.1:8890
  UPROPERTY(EditDefaultsOnly, Category = "Spotify")
  FString RedirectUri;

  UPROPERTY(EditDefaultsOnly, Category = "Spotify")
  uint16 Port;

  //Select the auth scopes needed (The default ones are enough for playing/pausing and fetching meta)
  UPROPERTY(EditDefaultsOnly, Category = "Spotify")
  TArray<ESpotifyApiScopes> AuthScopes;

  UPROPERTY(EditDefaultsOnly, Category = "Spotify")
  FString State;

  // How often is the service being pinged for updates (track, volume, etc...)
  UPROPERTY(BlueprintReadOnly, Category = "Spotify")
  float PollingInterval;

  // The Return you get once the acces has been granted! (default = success page + js closing that window/tab)
  UPROPERTY(EditDefaultsOnly, Category = "Spotify")
  FString WebReturn;

  USpotifyTokenSave* TokenSave;

  //Handles Polling the API
  FTimerHandle PollingTimer;

  //Handles retrieving new Access and Refresh Keys
  FTimerHandle RefreshTokenTimer;
  
  // The Gathered Auth key from the users app approval
  FString AuthKey;

  // The AccessToken retrieved using the AuthKey from Spotifys API
  FString AccessToken;

  // The RefreshToken retrieved using the AuthKey from Spotifys API (Needs to be updated for savegame usage)
  FString RefreshToken;

  FHttpModule* Http;

  // Playing/Paused state
  bool bIsPlaying;

protected:

  //Gets the AccessToken after successful auth
  void GetAccessToken();
  
  //Uses Refresh token to get a new pair of Access and Refresh Keys
  void UseRefreshToken();

  //Delegate Endpoint
  void OnAccessTokenReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

  void OnRefreshTokenUsed(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

  //On CurrentlyPlayed Received
  void OnCurrentSongReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
  
  //Get the String value from the Scopes
  FString GetScopeValue(ESpotifyApiScopes Scope);

  //Builds the Spotify Auth URL 
  FString BuildAuthURL();

  //BP Event for more Cosmetic things.
  UFUNCTION(BlueprintImplementableEvent, Category = "Spotify")
  void OnCurrentSong(
    const FString& SongName,
    const FString& ArtistName,
    const FString& AlbumName,
    const int32& Length,
    const int32& CurrentPos,
    const bool IsPlaying,
    const float& Volume,
    const FString& AlbumImage
  );

  void FetchCurrentSong();

  void PlaybackRequest(FString Url, FString Type);

  UFUNCTION(BlueprintCallable, Category = "Spotify")
  void RequestPause();

  UFUNCTION(BlueprintCallable, Category = "Spotify")
  void RequestNextSong();

  UFUNCTION(BlueprintCallable, Category = "Spotify")
  void RequestPrevSong();



private:

  // Listens for incoming connections
  FSocket * ListenerSocket;

  // Gets and serves Connections
  FSocket* ConnectionSocket;

  FIPv4Endpoint ConnectionRemoteAddress;
  
  //Timerhandle for handling the ListenerSocket
  FTimerHandle ListenerTimer;

  //TimerHandle for handling the ConnectionSocket
  FTimerHandle ConnectionTimer;

private:

  void StartTCPListener();

  void TCPSocketListener();

  void TCPConnectionHandler();

protected:

  virtual void BeginDestroy() override;

  UFUNCTION(BlueprintCallable, Category = "Spotify")
  void BeginAuth();

public:

  UFUNCTION(BlueprintCallable, Category = "Playback")
  void SetProgress(float InPercent);

  UFUNCTION(BlueprintCallable, Category = "Playback")
  void SetVolume(float InPercent);

  UFUNCTION(BlueprintCallable, Category = "Playback")
  void SetSong(FString SPContextURI);

protected:

  int32 PlaybackPos;
  int32 Duration;
  FString SpotifyUrl;


};
